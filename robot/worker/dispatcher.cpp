#include "dispatcher.hpp"
#include <assert.h>
#include <common/thread/run.hpp>
#include <common/message/event/flush.hpp>
#include <common/message/message.hpp>

#include <robot/thread/telegram/telegram.hpp>
#include <persistent/persistent.hpp>


namespace robot {
namespace worker {
std::unique_ptr<dispatcher> dispatcher::instance_;

void dispatcher::init(config const& conf) {
    // assert(!dispatcher::instance_);
    dispatcher::instance_.reset(new dispatcher());

    // it must be setted up before everything
    common::thread::setup_run_flag(dispatcher::instance_->run_);

    assert(conf.telegram_ && conf.persistent_);

    /*
        INIT ALL THREADS
    */
//    thread::telegram::init(conf.telegram_.bot_token_, conf.telegram_.ids_);
    persistent::init(conf.persistent_);

   instance_->event_loop_ = std::thread(&dispatcher::event_loop, instance_.get());


}

void dispatcher::clear() {
    dispatcher::instance_.reset();
}

void dispatcher::stop() {
    instance_->run_ = false;
}


// TODO: This ugly code must be removed
void dispatcher::event_loop() {
    delay_in_ms_ = 5000;
    while (instance_->run_) {
        auto flush_message = common::message::make<common::message::event::flush>();
        persistent::push_query(std::move(flush_message));
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_in_ms_));
    }
}

bool dispatcher::run() {
    return instance_->run_;
}

dispatcher::dispatcher() : run_(true) {
    //event_loop_ = std::thread(&dispatcher::event_loop, this);
}

dispatcher::~dispatcher() {
    if (event_loop_.joinable())
        event_loop_.join();
}
} // namespace worker
} // namespace robot