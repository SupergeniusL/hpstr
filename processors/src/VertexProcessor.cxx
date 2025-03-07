/** 
 * @file VertexProcessor.h
 * @brief Class used to convert LCIO Vertex collections into ROOT collections.
 * @author Cameron Bravo, SLAC
 */
#include "VertexProcessor.h" 
#include "utilities.h"

VertexProcessor::VertexProcessor(const std::string& name, Process& process)
    : Processor(name, process) { 
    }

VertexProcessor::~VertexProcessor() { 
}

void VertexProcessor::configure(const ParameterSet& parameters) {

    std::cout << "Configuring VertexProcessor" << std::endl;
    try
    {
        debug_             = parameters.getInteger("debug", debug_);
        vtxCollLcio_       = parameters.getString("vtxCollLcio", vtxCollLcio_);
        vtxCollRoot_       = parameters.getString("vtxCollRoot", vtxCollRoot_);
        partCollRoot_      = parameters.getString("partCollRoot", partCollRoot_);
        kinkRelCollLcio_   = parameters.getString("kinkRelCollLcio", kinkRelCollLcio_);
        trkRelCollLcio_    = parameters.getString("trkRelCollLcio", trkRelCollLcio_);
        trackStateLocation_= parameters.getString("trackStateLocation", trackStateLocation_);
        
    }
    catch (std::runtime_error& error)
    {
        std::cout << error.what() << std::endl;
    }
}

void VertexProcessor::initialize(TTree* tree) {
    // Add branches to tree
    tree->Branch(vtxCollRoot_.c_str(),  &vtxs_);
    tree->Branch(partCollRoot_.c_str(), &parts_);
}

bool VertexProcessor::process(IEvent* ievent) {

    if (debug_ > 0) std::cout << "VertexProcessor: Clear output vector" << std::endl;
    for(int i = 0; i < vtxs_.size(); i++) delete vtxs_.at(i);
    vtxs_.clear();
    for(int i = 0; i < parts_.size(); i++) delete parts_.at(i);
    parts_.clear();

    Event* event = static_cast<Event*> (ievent);

    // Get the collection of vertices from the LCIO event. If no such collection 
    // exist, a DataNotAvailableException is thrown
    if (debug_ > 0) std::cout << "VertexProcessor: Get LCIO Collection " << vtxCollLcio_ << std::endl;
    EVENT::LCCollection* lc_vtxs = nullptr;
    try
    {
        lc_vtxs = event->getLCCollection(vtxCollLcio_.c_str()); 
    }
    catch (EVENT::DataNotAvailableException e) 
    {
        std::cout << e.what() << std::endl;
        return false;
    }

    // Get the collection of LCRelations between GBL tracks and kink data and track data variables.
    EVENT::LCCollection* gbl_kink_data{nullptr};
    EVENT::LCCollection* track_data{nullptr};
        
    try
    {
        if (!kinkRelCollLcio_.empty())
            gbl_kink_data = static_cast<EVENT::LCCollection*>(event->getLCCollection(kinkRelCollLcio_.c_str()));
        if (!trkRelCollLcio_.empty())
            track_data = static_cast<EVENT::LCCollection*>(event->getLCCollection(trkRelCollLcio_.c_str()));
    }
    catch (EVENT::DataNotAvailableException e)
    {
        std::cout << e.what() << std::endl;
        if (!gbl_kink_data)
            std::cout<<"Failed retrieving " << kinkRelCollLcio_ <<std::endl;
        if (!track_data)
            std::cout<<"Failed retrieving " << trkRelCollLcio_ <<std::endl;
        
    }
    
    
    if (debug_ > 0) std::cout << "VertexProcessor: Converting Verteces" << std::endl;
    for (int ivtx = 0 ; ivtx < lc_vtxs->getNumberOfElements(); ++ivtx) 
    {
        if (debug_ > 0) std::cout << "VertexProcessor: Converting Vertex " << ivtx << std::endl;
        EVENT::Vertex * lc_vtx{nullptr};
        lc_vtx = static_cast<EVENT::Vertex*>(lc_vtxs->getElementAt(ivtx));

        if (debug_ > 0) std::cout << "VertexProcessor: Build Vertex" << std::endl;
        Vertex* vtx = utils::buildVertex(lc_vtx);

        if (debug_ > 0) std::cout << "VertexProcessor: Get Particles" << std::endl;
        std::vector<EVENT::ReconstructedParticle*> lc_parts = lc_vtx->getAssociatedParticle()->getParticles();
        for(auto lc_part : lc_parts)
        {
           if (debug_ > 0) std::cout << "VertexProcessor: Build particle" << std::endl;
           Particle * part = utils::buildParticle(lc_part,trackStateLocation_, gbl_kink_data, track_data);
           if (debug_ > 0) std::cout << "VertexProcessor: Add particle" << std::endl;
            parts_.push_back(part);
            vtx->addParticle(part);
        }

        if (debug_ > 0) std::cout << "VertexProcessor: Add Vertex" << std::endl;
        vtxs_.push_back(vtx);
    }

    if (debug_ > 0) std::cout << "VertexProcessor: End process" << std::endl;
    return true;
}

void VertexProcessor::finalize() { 
}

DECLARE_PROCESSOR(VertexProcessor); 
