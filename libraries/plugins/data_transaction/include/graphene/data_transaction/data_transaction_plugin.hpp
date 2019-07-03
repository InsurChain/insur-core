#pragma once

#include <graphene/app/plugin.hpp>
#include <graphene/chain/database.hpp>

namespace graphene { namespace data_transaction {
    using namespace chain;

class data_transaction_plugin : public graphene::app::plugin
{
  public:
    ~data_transaction_plugin() {}

    std::string plugin_name() const override;

    virtual void plugin_set_program_options(
        boost::program_options::options_description &command_line_options,
        boost::program_options::options_description &config_file_options) override;

    virtual void plugin_initialize(const boost::program_options::variables_map &options) override;
    virtual void plugin_startup() override;
    virtual void plugin_shutdown() override;

  private:
    void check_data_transaction(const graphene::chain::signed_block &b);

    uint32_t data_transaction_lifetime = 0;
};
} } //graphene::data_transaction_plugin
