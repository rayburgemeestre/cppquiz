/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "quiz_runner.h"

quiz_runner::quiz_runner(quiz the_quiz) : quiz_(std::move(the_quiz)) {}

void quiz_runner::add_participant(const std::string& unique_id, const std::string& nickname) {
  if (!participants_.contains(unique_id)) {
    participants_[unique_id] = participant(unique_id, nickname);
  } else {
    participants_[unique_id].update_nickname(nickname);
  }
  for (auto& callback : participant_callbacks_) {
    callback(nlohmann::json{
        {"msg", "add_participant"},
        {"value", unique_id},
    });
  }
}

void quiz_runner::set_participant_connected(const std::string& unique_id, bool value) {
  if (participants_.contains(unique_id)) {
    participants_[unique_id].set_connected(value);
  }
  for (auto& callback : quizmaster_callbacks_) {
    callback(unique_id);
    callback(nlohmann::json{
        {"msg", "add_participant"},
        {"value", unique_id},
    });
  }
}

void quiz_runner::start_quiz() {
  quiz_started_ = true;
  for (auto& callback : participant_callbacks_) {
    callback(nlohmann::json{
        {"msg", "start_quiz"},
        {"value", true},
    });
  }
}

void quiz_runner::stop_quiz() {
  quiz_started_ = false;
  for (auto& callback : participant_callbacks_) {
    callback(nlohmann::json{
        {"msg", "start_quiz"},
        {"value", false},
    });
  }
}

const std::map<std::string, participant>& quiz_runner::get_participants() {
  return participants_;
}

void quiz_runner::add_quizmaster_callback(std::function<void(nlohmann::json)> callback) {
  quizmaster_callbacks_.push_back(callback);
}

void quiz_runner::add_participant_callback(std::function<void(nlohmann::json)> callback) {
  participant_callbacks_.push_back(callback);
}

bool quiz_runner::quiz_started() const {
  return quiz_started_;
}