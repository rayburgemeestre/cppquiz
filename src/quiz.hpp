/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <string>
#include <vector>

class answer {
private:
  std::string answer_;
  bool is_correct_;

public:
  answer(std::string answer, bool is_correct) : answer_(std::move(answer)), is_correct_(is_correct) {}

  std::string get_answer() {
    return answer_;
  }

  bool is_correct() {
    return is_correct_;
  }
};

class question {
private:
  std::string question_;
  std::vector<answer> answers_;

public:
  question(std::string question, std::vector<answer> answers)
      : question_(std::move(question)), answers_(std::move(answers)) {}
};

class quiz {
private:
  std::string quiz_name_;
  std::vector<question> questions_;

public:
  quiz(std::string quiz_name, std::vector<question> questions)
      : quiz_name_(std::move(quiz_name)), questions_(std::move(questions)) {}
};