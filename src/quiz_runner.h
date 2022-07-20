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
  quiz quiz_;
  std::map<std::string, participant> participants_;

  std::vector<std::function<void(nlohmann::json)>> participant_callbacks_;
  std::vector<std::function<void(nlohmann::json)>> quizmaster_callbacks_;

  bool quiz_started_ = false;

public:
  quiz_runner(quiz the_quiz);

  void add_participant(const std::string& unique_id, const std::string& nickname);
  void set_participant_connected(const std::string& unique_id, bool value);
  void start_quiz();
  void stop_quiz();

  const decltype(participants_)& get_participants();

  void add_participant_callback(std::function<void(nlohmann::json)> callback);
  void add_quizmaster_callback(std::function<void(nlohmann::json)> callback);

  bool quiz_started() const;
};
