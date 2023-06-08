#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/Common/interface/View.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include <vector>
#include <iostream>

template <typename T>
class HitInfoTableProducer : public edm::stream::EDProducer<> {
 public:
  HitInfoTableProducer(edm::ParameterSet const& params)
    : name_(params.getParameter<std::string>("name")),
    doc_(params.getParameter<std::string>("doc")),
    src_(consumes<T>(params.getParameter<edm::InputTag>("src"))),
    cut_(params.getParameter<std::string>("cut"), true) {
    produces<nanoaod::FlatTable>();
  }

  ~HitInfoTableProducer() override {}

  unsigned int virtual layer(const typename T::value_type& hit) { 
    throw cms::Exception("HitInfoTableProducer:layer") << "Virtual function layer is not implemented!"; 
  }

  std::pair<int, int> virtual uv(const typename T::value_type& hit) { 
    throw cms::Exception("HitInfoTableProducer:uv") << "Virtual function uv coordinates is not implemented!"; 
  }

  void produce(edm::Event& iEvent, const edm::EventSetup& iSetup) override {

    edm::Handle<T> objs;
    iEvent.getByToken(src_, objs);

    std::vector<unsigned int> layervals;
    std::vector<int> uvals;
    std::vector<int> vvals;

    for (const auto& obj : *objs) {
      if (cut_(obj)) {
	layervals.emplace_back(layer(obj));
	uvals.emplace_back(uv(obj).first);
	vvals.emplace_back(uv(obj).second);
      }
    }

    auto tab = std::make_unique<nanoaod::FlatTable>(layervals.size(), name_, false, true);
    tab->addColumn<int>("u", uvals, "u cell coordinate");
    tab->addColumn<int>("v", vvals, "v cell coordinate");
    tab->addColumn<unsigned int>("layer", layervals, "layer coordinate");

    iEvent.put(std::move(tab));

  }

 protected:
  const std::string name_, doc_;
  const edm::EDGetTokenT<T> src_;
  const StringCutObjectSelector<typename T::value_type> cut_;

};
