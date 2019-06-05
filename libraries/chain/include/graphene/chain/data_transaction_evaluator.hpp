#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/data_transaction_object.hpp>

namespace graphene { namespace chain {

   class data_transaction_create_evaluator : public evaluator<data_transaction_create_evaluator>
    {
    public:
        typedef data_transaction_create_operation operation_type;

        void_result do_evaluate( const data_transaction_create_operation& o );
        object_id_type do_apply(const data_transaction_create_operation& o, uint32_t billed_cpu_time_us = 0);
    };

   class data_transaction_update_evaluator : public evaluator<data_transaction_update_evaluator>
    {
    public:
        typedef data_transaction_update_operation operation_type;

        void_result do_evaluate( const data_transaction_update_operation& o );
        void_result do_apply(const data_transaction_update_operation& o, uint32_t billed_cpu_time_us = 0);
    };

   class data_transaction_datasource_upload_evaluator : public evaluator<data_transaction_datasource_upload_evaluator>
    {
    public:
        typedef data_transaction_datasource_upload_operation operation_type;

        void_result do_evaluate( const data_transaction_datasource_upload_operation& o );
        void_result do_apply(const data_transaction_datasource_upload_operation& o, uint32_t billed_cpu_time_us = 0);
    };

   class data_transaction_datasource_validate_error_evaluator : public evaluator<data_transaction_datasource_validate_error_evaluator>
    {
    public:
        typedef data_transaction_datasource_validate_error_operation operation_type;

        void_result do_evaluate( const data_transaction_datasource_validate_error_operation& o );
        void_result do_apply(const data_transaction_datasource_validate_error_operation& o, uint32_t billed_cpu_time_us = 0);
    };

    class data_transaction_complain_evaluator : public evaluator<data_transaction_complain_evaluator>
    {
    public:
        typedef data_transaction_complain_operation operation_type;

        void_result do_evaluate(const data_transaction_complain_operation& op);
        object_id_type do_apply(const data_transaction_complain_operation& op, uint32_t billed_cpu_time_us = 0);
    };
} } // graphene::chain

