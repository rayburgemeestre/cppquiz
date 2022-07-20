/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <functional>
#include <map>
#include "nlohmann/json.hpp"
#include "participant.hpp"
#include "quiz.hpp"

class quiz_runner {
private:
  std::string quiz_id_;
  quiz quiz_;
  size_t current_question_ = std::numeric_limits<size_t>::max();
  std::map<std::string, participant> participants_;
  std::map<std::string, std::vector<std::tuple<std::string, int, int>>> participant_answers;

  std::vector<std::function<void(nlohmann::json)>> participant_callbacks_;
  std::vector<std::function<void(nlohmann::json)>> quizmaster_callbacks_;

  bool quiz_started_ = false;

public:
  quiz_runner(quiz the_quiz);

  void add_participant(const std::string& unique_id, const std::string& nickname);
  void set_participant_connected(const std::string& unique_id, bool value);
  void start_quiz(std::string quiz_id);
  void stop_quiz();
  void next_question();
  void set_answers(std::string participant_id, nlohmann::json answers);

  const decltype(participants_)& get_participants();

  void add_participant_callback(std::function<void(nlohmann::json)> callback);
  void add_quizmaster_callback(std::function<void(nlohmann::json)> callback);

  void send_participants_to_quizmaster();
  void send_answers_to_quizmaster();

  bool quiz_started() const;
  const std::string& quiz_id() const;
  size_t current_question() const;
  nlohmann::json current_question_json() const;
  size_t num_questions() const;
};
