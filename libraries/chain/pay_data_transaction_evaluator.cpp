
#include <graphene/chain/pay_data_transaction_evaluator.hpp>
#include <graphene/chain/data_transaction_object.hpp>
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/exceptions.hpp>
#include <graphene/chain/is_authorized_asset.hpp>
#include <graphene/chain/pocs_object.hpp>
#include <graphene/chain/hardfork.hpp>
#include <fc/uint128.hpp>

namespace graphene { namespace chain {

optional<account_object> pay_data_transaction_evaluator::get_account_by_name(const string& account_name)
{
    const auto& account_by_name = db().get_index_type<account_index>().indices().get<by_name>();
    auto itr = account_by_name.find(account_name);
    return itr == account_by_name.end()? optional<account_object>() : *itr;
}

share_type pay_data_transaction_evaluator::cut_fee(share_type a, uint16_t p)
{
   if( a == 0 || p == 0 )
      return 0;
   if( p == GRAPHENE_100_PERCENT )
      return a;

   fc::uint128 r(a.value);
   r *= p;
   r /= GRAPHENE_100_PERCENT;
   return r.to_uint64();
}

void pay_data_transaction_evaluator::update_alliance_pocs(alliance_id_type alliance_id, object_id_type product_id, const pay_data_transaction_operation& op)
{
    database& _db = db();
    alliance_object alliance_obj = _db.get(alliance_id);
    const auto& products = alliance_obj.data_products;
    uint16_t idx = std::distance(products.begin(),
            find_if(products.begin(), products.end(), [&](object_id_type p) { return p == product_id; })
            );
    uint64_t pocs_weight = (idx < alliance_obj.pocs_thresholds.size()) ? alliance_obj.product_pocs_weights[idx] : 1;

    auto& pocs_idx = _db.get_index_type<pocs_index>().indices().get<by_multi_id>();
    auto update_pocs = [&](account_id_type account, bool is_buy) {
        auto pocs = pocs_idx.find(boost::make_tuple(alliance_id, account));
        if (pocs == pocs_idx.end()) {
            _db.create<pocs_object>([&](pocs_object& b) {
                    if (is_buy) {
                        b.total_buy += pocs_weight;
                    } else {
                        b.total_sell += pocs_weight;
                    }
                    b.alliance_id = alliance_id;
                    b.account_id = account;
                    });
        } else {
            _db.modify(*pocs, [&](pocs_object& b) {
                    if (is_buy) {
                        b.total_buy += pocs_weight;
                    } else {
                        b.total_sell += pocs_weight;
                    }
                    });
        }
    };

    update_pocs(op.from, true);
    update_pocs(op.to, false);
}

void_result pay_data_transaction_evaluator::do_evaluate( const pay_data_transaction_operation& op )
{ try {
   const database& _db = db();

   const auto& data_transaction_idx = _db.get_index_type<data_transaction_index>().indices().get<by_request_id>();
   auto maybe_found = data_transaction_idx.find(op.request_id);
   if (maybe_found == data_transaction_idx.end()) {
       return void_result();
   }
   FC_ASSERT(data_transaction_status_confirmed == maybe_found->status, "dto status must be confirmed");

   const data_transaction_object& dto = *maybe_found;
   auto iter = std::find_if(dto.datasources_status.begin(), dto.datasources_status.end(),
           [&](const data_transaction_datasource_status_object& status_obj) {
               return status_obj.datasource == op.to && status_obj.status == data_transaction_datasource_status_uploaded;
           });
   FC_ASSERT(iter != dto.datasources_status.end(), "datasource ${d} not found or status not uploaded", ("d", op.to));
   FC_ASSERT(_db.find_object(dto.product_id) != nullptr, "product not found, product_id ${p}", ("p", dto.product_id));

   if (dto.alliance_id.valid()) {
       alliance_object alliance_obj = _db.get(*(dto.alliance_id));
       const auto& products = alliance_obj.data_products;
       uint16_t idx = std::distance(begin(products), find_if(begin(products), end(products),
                   [&](const alliance_data_product_id_type& product_id) {
                   return dto.product_id == product_id;
                   }));
       if (idx < products.size()) {
           FC_ASSERT(op.amount.amount == alliance_obj.prices[idx], "pay amount doesn't match product price");
       } else {
           FC_THROW("cannot find product price from alliance, product_id ${p}, alliance_id ${l}", ("p", dto.product_id)("l", *dto.alliance_id));
       }
   } else {
       auto product_id = static_cast<free_data_product_id_type>(dto.product_id);
       auto free_data_obj = _db.get(product_id);
       FC_ASSERT(op.amount.amount == free_data_obj.price, "pay amount doesn't match product price");
   }

   const account_object& from_account = op.from(_db);
   const account_object& to_account = op.to(_db);
   const asset_object&  asset_type = op.amount.asset_id(_db);

   try {
      GRAPHENE_ASSERT(
         is_authorized_asset( _db, from_account, asset_type ),
         transfer_from_account_not_whitelisted,
         "'from' account ${from} is not whitelisted for asset ${asset}",
         ("from",op.from)
         ("asset", op.amount.asset_id)
         );
      GRAPHENE_ASSERT(
         is_authorized_asset( _db, to_account, asset_type ),
         transfer_to_account_not_whitelisted,
         "'to' account ${to} is not whitelisted for asset ${asset}",
         ("to",op.to)
         ("asset", op.amount.asset_id)
         );

      if( asset_type.is_transfer_restricted() ) {
         GRAPHENE_ASSERT(
            from_account.id == asset_type.issuer || to_account.id == asset_type.issuer,
            transfer_restricted_transfer_asset,
            "Asset {asset} has transfer_restricted flag enabled",
            ("asset", op.amount.asset_id)
          );
      }

      bool insufficient_balance = _db.get_balance( from_account, asset_type ).amount >=  op.amount.amount;
      FC_ASSERT( insufficient_balance,
                 "Insufficient Balance: ${balance}, unable to transfer '${total_transfer}' from account '${a}' to '${t}'",
                 ("a",from_account.name)("t",to_account.name)("total_transfer",_db.to_pretty_string( op.amount ))("balance",_db.to_pretty_string(_db.get_balance(from_account, asset_type))) );

      return void_result();
   } FC_RETHROW_EXCEPTIONS( error, "Unable to pay_data_transaction ${a} from ${f} to ${t}", ("a",_db.to_pretty_string( op.amount ))("f",op.from(_db).name)("t",op.to(_db).name) );

}  FC_CAPTURE_AND_RETHROW( (op) ) }

operation_result pay_data_transaction_evaluator::do_apply(const pay_data_transaction_operation& op, int32_t billed_cpu_time_us)
{ try {
   database& _db = db();
   const auto& data_transaction_idx = _db.get_index_type<data_transaction_index>().indices().get<by_request_id>();
   auto maybe_found = data_transaction_idx.find(op.request_id);
   if (maybe_found == data_transaction_idx.end()) {
       return void_result();
   }
   const data_transaction_object& dto = *maybe_found;

   uint16_t commission_percent = GRAPHENE_DEFAULT_COMMISSION_PERCENT;
   if (_db.head_block_time() >= HARDFORK_1001_TIME) {
       auto rate_param = _db.get_commission_percent();
       commission_percent = dto.alliance_id.valid() ? rate_param.alliance_data_market_commission_percent : rate_param.free_data_market_commission_percent;
   }
   share_type commission_amount = cut_fee(op.amount.amount, commission_percent);
   share_type datasource_amount = op.amount.amount - commission_amount;

   _db.adjust_balance(op.from, -op.amount);
   _db.adjust_balance(op.to,  asset(datasource_amount));

   uint16_t reserve_percent_of_commission = 0;
   account_id_type commission_account = account_id_type();
   if (_db.head_block_time() >= HARDFORK_1003_TIME) {
       auto param = _db.get_commission_percent();
       reserve_percent_of_commission = param.reserve_percent;

       optional<account_object> account = get_account_by_name(param.commission_account);
       commission_account = account.valid() ? account->get_id() : account_id_type();
   }
   share_type reserved_cut = cut_fee(commission_amount, reserve_percent_of_commission);
   _db.adjust_balance(commission_account, asset(commission_amount - reserved_cut));
   _db.modify(dto, [&](data_transaction_object& obj) {
           for (auto& status_obj : obj.datasources_status) {
               if (status_obj.datasource == op.to)
                   status_obj.status = data_transaction_datasource_status_payed;
           }
           });

   if (dto.alliance_id.valid()) {
       update_alliance_pocs(*dto.alliance_id, dto.product_id, op);
   }

   return asset(transaction_fee);
} FC_CAPTURE_AND_RETHROW((op)) }

void pay_data_transaction_evaluator::prepare_fee(account_id_type account_id, asset fee, const operation& o)
{
    database& _db = db();
    auto& pay_op = o.get<pay_data_transaction_operation>();
    const auto& data_transaction_idx = _db.get_index_type<data_transaction_index>().indices().get<by_request_id>();
    auto dto = data_transaction_idx.find(pay_op.request_id);
    if (dto == data_transaction_idx.end()) {
        evaluator::prepare_fee(account_id, fee, o);
        return;
    }

    if (dto->alliance_id.valid() && _db.head_block_time() >= HARDFORK_1001_TIME) {

        uint16_t idx = 0;
        uint64_t pocs_threshold = 0;
        alliance_object alliance_obj = _db.get(*(dto->alliance_id));
        for(const auto& product : alliance_obj.data_products) {
            if (product == dto->product_id) {
                pocs_threshold = alliance_obj.pocs_thresholds[idx];
                break;
            }
            ++idx;
        }
        if (0 == pocs_threshold) {
            auto product_id = static_cast<alliance_data_product_id_type>(dto->product_id);
            alliance_data_product_object alliance_data_product_obj = _db.get(product_id);
            pocs_threshold = alliance_data_product_obj.pocs_threshold;
        }

        auto& index = _db.get_index_type<pocs_index>().indices().get<by_multi_id>();
        auto pocs = index.find(boost::make_tuple(*(dto->alliance_id), pay_op.from));
        if (pocs != index.end()) {
            if (pocs->total_buy + pocs->total_sell >= pocs_threshold) {
                fee.amount = fee.amount - fee.amount * (pocs->total_sell - pocs->total_buy) / (pocs->total_sell + pocs->total_buy);
                if (pocs->total_buy > pocs->total_sell && idx < alliance_obj.fee_bases.size() &&
                        alliance_obj.fee_bases[idx] > 1) {
                    fee.amount = fee.amount * alliance_obj.fee_bases[idx];
                }
            }
        }
    }
    transaction_fee = fee.amount.value;
    evaluator::prepare_fee(account_id, fee, o);
}

} } // graphene::chain
