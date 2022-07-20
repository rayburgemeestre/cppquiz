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

  std::string get_answer() const {
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

  std::string get_question() const {
    return question_;
  }

  std::vector<std::string> answers() const {
    // important not to include the info about which is correct here
    std::vector<std::string> the_answers;
    for (const auto& answer : answers_) {
      the_answers.push_back(answer.get_answer());
    }
    return the_answers;
  }
};

class quiz {
private:
  std::string quiz_name_;
  std::vector<question> questions_;

public:
  quiz(std::string quiz_name, std::vector<question> questions)
      : quiz_name_(std::move(quiz_name)), questions_(std::move(questions)) {}

  question get_question(size_t question_number) const {
    return questions_[question_number];
  }

  const std::vector<question>& questions() const {
    return questions_;
  }
};