/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <functional>
#include <map>
#include "participant.hpp"
#include "quiz.hpp"

class quiz_runner {
private:
  quiz quiz_;
  std::map<std::string, participant> participants_;

  std::vector<std::function<void(std::string)>> on_new_participant_callbacks_;

public:
  quiz_runner(quiz the_quiz);

  void add_participant(const std::string& unique_id, const std::string& nickname);
  void set_participant_connected(const std::string& unique_id, bool value);

  const decltype(participants_)& get_participants();

  void on_new_participant(std::function<void(std::string)> callback);
};
