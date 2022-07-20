/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "quiz_runner.h"
#include "util/logger.h"

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
    callback(nlohmann::json{
        {"msg", "add_participant"},
        {"value", unique_id},
    });
  }
}

void quiz_runner::start_quiz(std::string quiz_id) {
  quiz_id_ = std::move(quiz_id);
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

void quiz_runner::next_question() {
  if (current_question_ == std::numeric_limits<size_t>::max()) {
    current_question_ = 0;
  } else {
    current_question_++;
  }
  auto question_json = current_question_json();
  question_json["msg"] = "next_question";
  for (auto& callback : participant_callbacks_) {
    callback(nlohmann::json{
        {"msg", "set_question"},
        {"value", question_json},
    });
  }
  for (auto& callback : quizmaster_callbacks_) {
    callback(nlohmann::json{
        {"msg", "set_question"},
        {"value", question_json},
    });
  }
}

void quiz_runner::set_answers(std::string participant_id, nlohmann::json answers) {
  logger(INFO) << "Setting answers" << std::endl;

  participant_answers[participant_id].clear();
  for (const auto& item : answers.items()) {
    for (const auto& item2 : item.value().items()) {
      std::string quiz_id = item.key();
      int question_id = std::stoi(item2.key());
      int answer = item2.value().get<int>();
      participant_answers[participant_id].emplace_back(quiz_id, question_id, answer);
    }
  }

  send_answers_to_quizmaster();
}

void quiz_runner::send_participants_to_quizmaster() {
  nlohmann::json participants;
  for (const auto& [_, participant] : get_participants()) {
    participants.push_back(nlohmann::json{{"unique_id", participant.unique_id()},
                                          {"nickname", participant.nickname()},
                                          {"connected", participant.connected()}});
  }
  for (auto& callback : quizmaster_callbacks_) {
    callback(nlohmann::json{{"msg", "participants"}, {"value", participants}});
  }
}

void quiz_runner::send_answers_to_quizmaster() {
  nlohmann::json update;
  for (const auto& [participant_id, answers] : participant_answers) {
    for (const auto& [quiz_id, question_id, answer] : answers) {
      if (quiz_id != quiz_id_) {
        continue;
      }
      update.push_back(nlohmann::json{
          {"participant_id", participant_id},
          {"question_id", question_id},
          {"answer", answer},
      });
    }
  }
  for (auto& callback : quizmaster_callbacks_) {
    callback(nlohmann::json{{"msg", "answers_updated"}, {"value", update}});
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

const std::string& quiz_runner::quiz_id() const {
  return quiz_id_;
}

size_t quiz_runner::current_question() const {
  return current_question_;
}

nlohmann::json quiz_runner::current_question_json() const {
  if (current_question_ >= quiz_.questions().size()) {
    return nlohmann::json{};
  }
  return nlohmann::json{
      {"quiz_id", quiz_id_},
      {"question_id", current_question_},
      {"question", quiz_.get_question(current_question_).get_question()},
      {"answers", quiz_.get_question(current_question_).answers()},
  };
}

size_t quiz_runner::num_questions() const {
  return quiz_.questions().size();
}
