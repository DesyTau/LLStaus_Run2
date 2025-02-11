// -*- C++ -*-
//
// Package:    PhysicsTools/NanoAOD
// Class:      IsFromPatTauMapProducer
//
/**\class IsFromPatTauMapProducer IsFromPatTauMapProducer.cc PhysicsTools/NanoAOD/plugins/IsFromPatTauMapProducer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Maria Giulia Ratti (ETHZ) [mratti]
//         Created:  Thu, 22 Nov 2018 12:34:48 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/IsolatedTrack.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"

#include "LLStaus_Run2/Production/interface/Utils.h"

//
// class declaration
//

class IsFromPatTauMapProducer : public edm::global::EDProducer<>
{
    public:
        explicit IsFromPatTauMapProducer(const edm::ParameterSet& iConfig):
            pc_(consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("packedPFCandidates"))), // pf candidates
            tt_(consumes<pat::TauCollection>(iConfig.getParameter<edm::InputTag>("patTaus"))) // taus
        {
            produces<edm::ValueMap<int>>("isTauSignalCand"); // name of the value map that I want to actually produce
            produces<edm::ValueMap<int>>("isTauIsoCand"); // name of the value map that I want to actually produce
            produces<edm::ValueMap<int>>("isTauLeadChHadCand"); // name of the value map that I want to actually produce
        }
        ~IsFromPatTauMapProducer() override {};
        
        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
        
    private:
        void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;
        
        
        // ----------member data ---------------------------
        edm::EDGetTokenT<pat::PackedCandidateCollection> pc_;
        edm::EDGetTokenT<pat::TauCollection> tt_;
};

//
// constants, enums and typedefs
//


//
// static data member definitions
//

//
// member functions
//

// ------------ method called to produce the data  ------------
void IsFromPatTauMapProducer::produce(edm::StreamID streamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const
{
    // packedPFCandidate collection
    edm::Handle<pat::PackedCandidateCollection> pc_handle;
    iEvent.getByToken( pc_, pc_handle );
    
    // tau collection
    edm::Handle<pat::TauCollection> tau_handle;
    iEvent.getByToken( tt_, tau_handle );
    
    // the map cannot be filled straight away, so create an intermediate vector
    unsigned int Npc = pc_handle->size();
    std::vector<int> v_isTauSignalCand(Npc, -1);
    std::vector<int> v_isTauIsoCand(Npc, -1);
    std::vector<int> v_isTauLeadChHadCand(Npc, -1);
    
    unsigned int Ntau = tau_handle->size();
    
    for (unsigned int ipc=0; ipc<Npc; ipc++)
    {
        const auto &pc = pc_handle->at(ipc);
        
        int isTauSignalCand = -1;
        int isTauIsoCand = -1;
        int isTauLeadChHadCand = -1;
        
        for (unsigned int itau=0; itau<Ntau; itau++)
        {
            const auto &tau = tau_handle->at(itau);
            
            // Only check if not already matched
            if (isTauSignalCand < 0 && Utils::isTauSignalCand(tau, pc))
            {
                isTauSignalCand = itau;
            }
            
            if (isTauIsoCand < 0 && Utils::isTauIsoCand(tau, pc))
            {
                isTauIsoCand = -1;
            }
            
            if (isTauLeadChHadCand < 0 && Utils::isTauLeadChHadCand(tau, pc))
            {
                isTauLeadChHadCand = -1;
            }
        }
        
        //if (isTauSignalCand >= 0 || isTauIsoCand >= 0 || isTauLeadChHadCand >= 0)
        //{
        //    printf("Ntau %d, ipc %d, isTauSignalCand %d, isTauIsoCand %d, isTauLeadChHadCand %d \n", (int) Ntau, (int) ipc, isTauSignalCand, isTauIsoCand, isTauLeadChHadCand);
        //}
        
        v_isTauSignalCand[ipc] = isTauSignalCand;
        v_isTauIsoCand[ipc] = isTauIsoCand;
        v_isTauLeadChHadCand[ipc] = isTauLeadChHadCand;
    }
    
    
    std::unique_ptr<edm::ValueMap<int>> vm_isTauSignalCand(new edm::ValueMap<int>());
    edm::ValueMap<int>::Filler filler_isTauSignalCand(*vm_isTauSignalCand);
    filler_isTauSignalCand.insert(pc_handle, v_isTauSignalCand.begin(), v_isTauSignalCand.end());
    filler_isTauSignalCand.fill();
    iEvent.put(std::move(vm_isTauSignalCand), "isTauSignalCand");
    
    
    std::unique_ptr<edm::ValueMap<int>> vm_isTauIsoCand(new edm::ValueMap<int>());
    edm::ValueMap<int>::Filler filler_isTauIsoCand(*vm_isTauIsoCand);
    filler_isTauIsoCand.insert(pc_handle, v_isTauIsoCand.begin(), v_isTauIsoCand.end());
    filler_isTauIsoCand.fill();
    iEvent.put(std::move(vm_isTauIsoCand), "isTauIsoCand");
    
    
    std::unique_ptr<edm::ValueMap<int>> vm_isTauLeadChHadCand(new edm::ValueMap<int>());
    edm::ValueMap<int>::Filler filler_isTauLeadChHadCand(*vm_isTauLeadChHadCand);
    filler_isTauLeadChHadCand.insert(pc_handle, v_isTauLeadChHadCand.begin(), v_isTauLeadChHadCand.end());
    filler_isTauLeadChHadCand.fill();
    iEvent.put(std::move(vm_isTauLeadChHadCand), "isTauLeadChHadCand");
    
}



// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void IsFromPatTauMapProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{

    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("packedPFCandidates")->setComment("packed PF Candidates collection ");
    desc.add<edm::InputTag>("patTaus")->setComment("tau collection");
    
    descriptions.addWithDefaultLabel(desc);
    
}

//define this as a plug-in
DEFINE_FWK_MODULE(IsFromPatTauMapProducer);
