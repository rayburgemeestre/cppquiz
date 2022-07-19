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
  for (auto& callback : on_new_participant_callbacks_) {
    callback(unique_id);
  }
}

void quiz_runner::set_participant_connected(const std::string& unique_id, bool value) {
  if (participants_.contains(unique_id)) {
    participants_[unique_id].set_connected(value);
  }
  for (auto& callback : on_new_participant_callbacks_) {
    callback(unique_id);
  }
}

const std::map<std::string, participant>& quiz_runner::get_participants() {
  return participants_;
}

void quiz_runner::on_new_participant(std::function<void(std::string)> callback) {
  on_new_participant_callbacks_.push_back(callback);
}
