#ifndef __ZBICUTFLOW_ANAPROCESSOR_H__
#define __ZBICUTFLOW_ANAPROCESSOR_H__

// HPSTR
#include "Processor.h"
#include "ZBiHistos.h"
#include "IterativeCutSelector.h"
#include "SimpEquations.h"
#include "MutableTTree.h"

// ROOT 
#include "TFile.h"
#include "TTree.h"
#include "TRefArray.h"
#include "TBranch.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TEfficiency.h"
#include "TGraph.h"

// C++ 
#include <memory>

class ZBiCutflowProcessor : public Processor {

    public:

        ZBiCutflowProcessor(const std::string& name, Process& process);

        ~ZBiCutflowProcessor();

        virtual void configure(const ParameterSet& parameters);

        virtual void initialize(TTree* tree) {};

        virtual bool process(IEvent* event) {};

        virtual void finalize();

        virtual bool process();

        virtual void initialize(std::string inFilename, std::string outFilename);

        double calculateZBi(double n_on, double n_off, double tau);

        void printZBiMatrix();

        bool failPersistentCuts(MutableTTree* MTT);

        bool failTestCut(std::string cutname, MutableTTree* MTT);

        bool failTestCut(std::string cutname, std::map<std::string,double*> tuple);

        void getVdSimZ();

        void writeGraph(TFile* outF, std::string folder, TGraph* g);

        double round(double var);

        //get signal truth vtx z distribution. Used to calculate expected
        //signal
        void getSignalMCAnaVtxZ_h(std::string signalMCAnaFilename, std::string signal_pdgid);

        void testImpactParameterCut(MutableTTree* MTT, ZBiHistos* histos);

        void filterCuts();


    private:

        //Histograms
        std::shared_ptr<ZBiHistos> signalHistos_;
        std::shared_ptr<ZBiHistos> bkgHistos_;
        std::shared_ptr<ZBiHistos> testCutHistos_;
        std::shared_ptr<ZBiHistos> processorHistos_;
        /*
        ZBiHistos* signalHistos_{nullptr};
        ZBiHistos* bkgHistos_{nullptr};
        ZBiHistos* cutHistos_{nullptr};
        ZBiHistos* debugHistos_{nullptr};
        ZBiHistos* summaryHistos_{nullptr};
        */

        std::map<std::string,double> mcScale_;
        int debug_{0}; 
        std::string outFileName_{"zbi_out.root"};
        TFile* outFile_{nullptr};
        double vdMassMeV_;
        double ApMassMeV_;

        //cuts
        std::string cuts_cfgFile_{""};
        typedef std::map<std::string, std::pair<double,int>>::iterator cut_iter_;
        std::vector<std::string> cutVariables_;
        std::map<std::string,double> initialIntegrals_;
        std::map<std::string,std::vector<std::pair<double,double>>> global_ZBi_map_;

        //Cuts
        IterativeCutSelector *testCutsSelector_{nullptr};
        std::map<std::string, std::pair<double,int>>* testCutsPtr_;
        IterativeCutSelector *persistentCutsSelector_{nullptr};
        std::map<std::string, std::pair<double,int>>* persistentCutsPtr_;


        //signal

        //background
        std::string tritrigFilename_{""};
        std::map<std::string,double*> tritrig_tuple_;
        TTree* tritrigTree_{nullptr};

        //simp equations
        SimpEquations* simpEqs_{nullptr};

        //mass window
        double highMass_;
        double lowMass_;

        // Signal //
        std::string signalHistCfgFilename_{""};
        std::string signalVtxAnaFilename_{""};
        std::string signalVtxAnaTreename_{""};
        std::string signalMCAnaFilename_{""};
        std::string signal_pdgid_{""};
        TH1F* signalSimZ_h_{nullptr};
        MutableTTree* signalMTT_{nullptr};
        double signal_sf_ = 1.0;
        double signal_mass_MeV_;
        double massRes_MeV_;
        double radFrac_;
        double simp_radAcc_ = 0.0;
        double dNdm_;
        double logEps2_;

        // Background //
        std::string bkgVtxAnaFilename_{""};
        std::string bkgVtxAnaTreename_{""};
        MutableTTree* bkgMTT_{nullptr};

        double luminosity_;
        double tritrig_sf_;

        double ztail_nevents_ = 1.0;
        bool scan_zcut_ = false;
        double step_size_ = 0.01;

        // ZAlpha Cut Variable 
        double zalpha_slope_;;

        //Dev
        bool testSpecialCut_ = false;
};



#endif
