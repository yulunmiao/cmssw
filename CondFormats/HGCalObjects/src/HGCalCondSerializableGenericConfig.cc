/****************************************************************************
 *
 * This is a part of HGCal offline software.
 * Authors:
 *   Laurent Forthomme
 *
 ****************************************************************************/

#include <algorithm>

#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableGenericConfig.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

HGCalCondSerializableGenericConfig::HGCalCondSerializableGenericConfig() {}

HGCalCondSerializableGenericConfig::~HGCalCondSerializableGenericConfig() {}

std::vector<std::string> HGCalCondSerializableGenericConfig::keys() const {
  std::vector<std::string> keys;
  std::transform(params_.begin(), params_.end(), std::back_inserter(keys), [](const auto& pair) { return pair.first; });
  std::sort(keys.begin(), keys.end());
  return keys;
}

const std::vector<int>& HGCalCondSerializableGenericConfig::parameters(const std::string& key) const {
  if (params_.count(key) == 0)
    throw cms::Exception("HGCalCondSerializableGenericConfig") << "Failed to retrieve a parameter with key='" << key << "'.";
  return params_.at(key);
}

HGCalCondSerializableGenericConfig& HGCalCondSerializableGenericConfig::addParameter(const std::string& key, const std::vector<int>& values) {
  if (params_.count(key) != 0)
    edm::LogWarning("HGCalCondSerializableGenericConfig")
        << "Parameter with key='" << key << "' was already present in collection.";
  params_[key] = values;
  return *this;
}

std::ostream& operator<<(std::ostream& os, const HGCalCondSerializableGenericConfig& cond) {
  os << "HGCalCondSerializableGenericConfig{";
  std::string sep1;
  for (const auto& key : cond.keys()) {
    os << sep1 << key << ": {";
    std::string sep2;
    for (const auto& param : cond.parameters(key))
      os << sep2 << param, sep2 = ", ";
    os << "}";
    sep1 = ", ";
  }
  return os << "}";
}
