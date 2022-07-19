/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <string>

class participant {
private:
  std::string unique_id_;
  std::string nickname_;
  bool connected_ = true;

public:
  participant() = default;
  participant(std::string unique_id, std::string nickname)
      : unique_id_(std::move(unique_id)), nickname_(std::move(nickname)) {}

  void update_nickname(const std::string& nickname) {
    nickname_ = nickname;
  }

  std::string unique_id() const {
    return unique_id_;
  }

  std::string nickname() const {
    return nickname_;
  }

  bool connected() const {
    return connected_;
  }

  void set_connected(bool value) {
    connected_ = value;
  }
};