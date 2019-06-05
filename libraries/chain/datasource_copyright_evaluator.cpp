#include <graphene/chain/datasource_copyright_evaluator.hpp>
#include <graphene/chain/database.hpp>

namespace graphene { namespace chain {

void_result datasource_copyright_clear_evaluator::do_evaluate( const datasource_copyright_clear_operation& op )
{ try {
    FC_ASSERT(trx_state->_is_proposed_trx);
    return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }


void_result datasource_copyright_clear_evaluator::do_apply(const datasource_copyright_clear_operation& op, uint32_t billed_cpu_time_us)
{ try {
    database& _db = db();
    const auto& datasource_copyright_idx = _db.get_index_type<datasource_copyright_index>().indices().get<by_multi_id>();
    const auto range = datasource_copyright_idx.equal_range(boost::make_tuple(op.datasource_account, op.product_id));
    for (const datasource_copyright_object& obj : boost::make_iterator_range(range.first, range.second)) {
        _db.remove(obj);
    }
    return void_result();
} FC_CAPTURE_AND_RETHROW((op)) }

}}
