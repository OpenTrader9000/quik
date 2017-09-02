#include "dispatcher.hpp"
#include <assert.h>

namespace robot {
namespace worker {
std::unique_ptr<dispatcher> dispatcher::instance_;

void dispatcher::init(config const& conf) {
    // assert(!dispatcher::instance_);
    dispatcher::instance_.reset(new dispatcher());
    dispatcher::instance_->init_internal(conf);
}

void dispatcher::init_internal(config const& conf) {
    bot_thread_ =
    std::make_shared<telegram_ptr_t::element_type>(conf.telegram_.bot_token_, conf.telegram_.ids_);
}

void dispatcher::clear() {
    dispatcher::instance_.reset();
}

void dispatcher::stop() {
    instance_->run_ = false;
}

bool dispatcher::run() {
    return instance_->run_;
}

dispatcher::dispatcher() {
}

dispatcher::~dispatcher() {
}
} // namespace worker
} // namespace robot