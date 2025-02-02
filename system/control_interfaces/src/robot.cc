#include "mumei/control_interfaces/robot.h"

namespace mumei {

Robot::Robot(std::unique_ptr<RobotConfiguration> config)
  : GenericComponent(std::move(config)), MovingGroup(),
    model_(std::make_shared<multibody::Model>()) {}

Robot::Robot()
  : Robot::Robot(std::make_unique<RobotConfiguration>()) {}

Robot::Robot(std::unique_ptr<RobotConfiguration> config,
             std::shared_ptr<multibody::Model> model)
  : GenericComponent(std::move(config)), MovingGroup(),
    model_(std::make_shared<multibody::Model>()) {}

Robot::Robot(std::shared_ptr<multibody::Model> model)
  : Robot::Robot(std::make_unique<RobotConfiguration>(), std::move(model)) {}

void Robot::Initialize() {
  // Initialize all motors
  for (auto& component : moving_components_) {
    auto motor = std::dynamic_pointer_cast<GenericComponent>(
      component);
    if (motor != nullptr) {
      motor->Initialize();
    }
  }
  // Initialize all sensors
  for (auto& sp : non_joint_state_providers_) {
    auto sensor = std::dynamic_pointer_cast<GenericComponent>(sp);
    if (sensor != nullptr) {
      sensor->Initialize();
    }
  }
  for (auto& sp : joint_state_providers_) {
    auto sensor = std::dynamic_pointer_cast<GenericComponent>(sp);
    if (sensor != nullptr) {
      sensor->Initialize();
    }
  }
}

void Robot::SetUp() {
  // Initialize all motors
  for (auto& component : moving_components_) {
    auto motor = std::dynamic_pointer_cast<GenericComponent>(
      component);
    if (motor != nullptr) {
      motor->SetUp();
    }
  }
  // Initialize all sensors
  for (auto& sp : non_joint_state_providers_) {
    auto sensor = std::dynamic_pointer_cast<GenericComponent>(sp);
    if (sensor != nullptr) {
      sensor->SetUp();
    }
  }
  for (auto& sp : joint_state_providers_) {
    auto sensor = std::dynamic_pointer_cast<GenericComponent>(sp);
    if (sensor != nullptr) {
      sensor->SetUp();
    }
  }
}

void Robot::Terminate() {
  // Initialize all motors
  for (auto& component : moving_components_) {
    auto motor = std::dynamic_pointer_cast<GenericComponent>(
      component);
    if (motor != nullptr) {
      motor->Terminate();
    }
  }
  // Initialize all sensors
  for (auto& sp : non_joint_state_providers_) {
    auto sensor = std::dynamic_pointer_cast<GenericComponent>(sp);
    if (sensor != nullptr) {
      sensor->Terminate();
    }
  }
  for (auto& sp : joint_state_providers_) {
    auto sensor = std::dynamic_pointer_cast<GenericComponent>(sp);
    if (sensor != nullptr) {
      sensor->Terminate();
    }
  }
}

void Robot::RegisterStateProvider(
  std::shared_ptr<StateProvider> state_provider,
  bool is_joint_state_provider) {
  assert(!state_provider->IsRegistered());
  if (!is_joint_state_provider) {
    non_joint_state_providers_.push_back(state_provider);
  } else {
    joint_state_providers_.push_back(state_provider);
  }
  // register the index
  registered_components_.push_back(state_provider);
  name_to_index_[state_provider->name()] = registered_components_.size() - 1;
  state_provider->SetIndex(registered_components_.size() - 1);
}

void Robot::UpdateAllStates() {
  for (auto& state_provider : non_joint_state_providers_) {
    state_provider->RequestStateUpdate();
  }
  model_->UpdateJointStates();
}

void Robot::UpdateJointStates() {
  model_->UpdateJointStates();
}

const Eigen::VectorBlock<const Eigen::VectorXd>
Robot::GetJointPositions() const {
  return model_->GetPositions();
}

const Eigen::VectorBlock<const Eigen::VectorXd>
Robot::GetJointVelocities() const {
  return model_->GetVelocities();
}

std::weak_ptr<Indexable> Robot::GetComponent(const std::string& name) const {
  return registered_components_[name_to_index_.at(name)];
}

std::weak_ptr<Indexable> Robot::GetComponent(Index id) const {
  return registered_components_[id];
}

}  // namespace mumei
