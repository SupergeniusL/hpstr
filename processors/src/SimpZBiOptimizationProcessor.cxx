#include "SimpZBiOptimizationProcessor.h"
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>

SimpZBiOptimizationProcessor::SimpZBiOptimizationProcessor(const std::string& name, Process& process) 
    : Processor(name,process) {
    std::cout << "[SimpZBiOptimizationProcessor] Constructor()" << std::endl;
}

SimpZBiOptimizationProcessor::~SimpZBiOptimizationProcessor(){}

void SimpZBiOptimizationProcessor::configure(const ParameterSet& parameters) {
    std::cout << "[SimpZBiOptimizationProcessor] configure()" << std::endl;
    try
    {
        //Basic config
        debug_ = parameters.getInteger("debug",debug_);
        max_iteration_ = parameters.getInteger("max_iteration", max_iteration_);
        year_ = parameters.getInteger("year",year_);
        cuts_cfgFile_ = parameters.getString("cuts_cfgFile",cuts_cfgFile_);
        outFileName_ = parameters.getString("outFileName",outFileName_);
        cutVariables_ = parameters.getVString("cutVariables", cutVariables_);
        min_ztail_events_ = parameters.getDouble("ztail_events",min_ztail_events_);
        scan_zcut_ = parameters.getInteger("scan_zcut",scan_zcut_);
        step_size_ = parameters.getDouble("step_size",step_size_);
        eq_cfgFile_ = parameters.getString("eq_cfgFile", eq_cfgFile_);

        //Background
        bkgVtxAnaFilename_ = parameters.getString("bkgVtxAnaFilename", bkgVtxAnaFilename_);
        bkgVtxAnaTreename_ = parameters.getString("bkgVtxAnaTreename",bkgVtxAnaTreename_);
        background_sf_ = parameters.getDouble("background_sf",background_sf_);

        //MC Signal
        variableHistCfgFilename_ = 
            parameters.getString("variableHistCfgFilename",variableHistCfgFilename_);
        signalVtxAnaFilename_ = 
            parameters.getString("signalVtxAnaFilename", signalVtxAnaFilename_);
        signalVtxAnaTreename_ = 
            parameters.getString("signalVtxAnaTreename",signalVtxAnaTreename_);
        signalMCAnaFilename_ =
            parameters.getString("signalMCAnaFilename",signalMCAnaFilename_);
        signal_pdgid_ =
            parameters.getString("signal_pdgid", signal_pdgid_);
        signal_sf_ = parameters.getDouble("signal_sf", signal_sf_);
        signal_mass_ = parameters.getDouble("signal_mass",signal_mass_);
        mass_window_nsigma_ = parameters.getDouble("mass_window_nsigma", mass_window_nsigma_);

        //Expected Signal Calculation
        logEps2_ = parameters.getDouble("logEps2",logEps2_);

        // New Variables //
        new_variables_ = parameters.getVString("add_new_variables", new_variables_);
        new_variable_params_ = parameters.getVDouble("new_variable_params", new_variable_params_);

        //Dev
        testSpecialCut_ = parameters.getInteger("testSpecialCut", testSpecialCut_);

    }
    catch (std::runtime_error& error)
    {
        std::cout << error.what() << std::endl;
    }
}

void SimpZBiOptimizationProcessor::addNewVariables(MutableTTree* MTT, std::string variable, double param){
    if(variable == "zalpha"){
        MTT->addVariableZalpha(param);
    }
    else if(variable == "zbravo"){
        MTT->addVariableZbravo();
    }
    else if(variable == "zbravoalpha"){
        MTT->addVariableZbravoAlpha(param);
    }
    else if(variable == "zbravosum"){
        MTT->addVariableZbravosum();
    }
    else if(variable == "zbravosumalpha"){
        MTT->addVariableZbravosumAlpha(param);
    }
    else
        std::cout << "[SimpZBiOptimization]::ERROR::NEW VARIABLE " << variable << " IS NOT DEFINED IN MutableTTree.cxx"
            <<std::endl;
}

void SimpZBiOptimizationProcessor::fillEventHistograms(std::shared_ptr<ZBiHistos> histos, MutableTTree* MTT){
    
    //histos->Fill histograms for each variable defined in tree
    std::vector<std::string> variables = MTT->getAllVariables();
    for(std::vector<std::string>::iterator it=variables.begin(); it != variables.end(); it++) {
        std::string var = *it;
        histos->Fill1DHisto(var+"_h", MTT->getValue(var));
    }

    //always fill
    if(MTT->variableExists("unc_vtx_z") && MTT->variableExists("unc_vtx_ele_track_z0") 
            && MTT->variableExists("unc_vtx_pos_track_z0")){
        histos->Fill2DHisto("z0_v_recon_z_hh",MTT->getValue("unc_vtx_z"),MTT->getValue("unc_vtx_ele_track_z0"));
        histos->Fill2DHisto("z0_v_recon_z_hh",MTT->getValue("unc_vtx_z"),MTT->getValue("unc_vtx_pos_track_z0"));
    }

    //Investigating new variables
    if(MTT->variableExists("unc_vtx_ele_track_zalpha") && MTT->variableExists("unc_vtx_pos_track_zalpha")){
        histos->Fill2DHisto("z0_v_recon_zalpha_hh",MTT->getValue("unc_vtx_ele_track_zalpha"),MTT->getValue("unc_vtx_ele_track_z0"));
        histos->Fill2DHisto("z0_v_recon_zalpha_hh",MTT->getValue("unc_vtx_pos_track_zalpha"),MTT->getValue("unc_vtx_pos_track_z0"));
    }
    if(MTT->variableExists("unc_vtx_ele_zbravoalpha") && MTT->variableExists("unc_vtx_pos_zbravoalpha")){
        histos->Fill2DHisto("zbravo_v_zbravoalpha_hh",MTT->getValue("unc_vtx_ele_zbravoalpha"),MTT->getValue("unc_vtx_ele_zbravo"));
        histos->Fill2DHisto("zbravo_v_zbravoalpha_hh",MTT->getValue("unc_vtx_pos_zbravoalpha"),MTT->getValue("unc_vtx_pos_zbravo"));
    }
    if(MTT->variableExists("unc_vtx_ele_zbravo") && MTT->variableExists("unc_vtx_pos_zbravo")){
        histos->Fill2DHisto("zbravo_v_recon_z_hh",MTT->getValue("unc_vtx_z"),MTT->getValue("unc_vtx_ele_zbravo"));
        histos->Fill2DHisto("zbravo_v_recon_z_hh",MTT->getValue("unc_vtx_z"),MTT->getValue("unc_vtx_pos_zbravo"));
    }
    if(MTT->variableExists("unc_vtx_zbravosum")){
        histos->Fill2DHisto("zbravosum_v_recon_z_hh",MTT->getValue("unc_vtx_z"),MTT->getValue("unc_vtx_zbravosum"));
        histos->Fill2DHisto("zbravosum_v_zbravosumalpha_hh",MTT->getValue("unc_vtx_zbravosumalpha"),MTT->getValue("unc_vtx_zbravosum"));
    }
}

void SimpZBiOptimizationProcessor::initialize(std::string inFilename, std::string outFilename){
    std::cout << "[SimpZBiOptimizationProcessor] Initialize" << std::endl;

    //Load Simp Equations
    simpEqs_ = new SimpEquations(year_, eq_cfgFile_);
    massResolution_ = simpEqs_->massResolution(signal_mass_);

    //Define Mass window
    lowMass_ = signal_mass_ - mass_window_nsigma_*massResolution_/2.0;
    highMass_ = signal_mass_ + mass_window_nsigma_*massResolution_/2.0;
    std::cout << "[SimpZBiOptimization]::Mass Window: " << lowMass_ << " - " << highMass_ << std::endl;

    //Initialize output file
    std::cout << "[SimpZBiOptimization]::Output File: " << outFileName_.c_str() << std::endl;
    outFile_ = new TFile(outFileName_.c_str(),"RECREATE");

    //Get the signal pretrigger simulated vertex z distribution
    std::cout << "[SimpZBiOptimization]::Getting MC Signal pre-trigger vertex z distribution from file "
        << signalMCAnaFilename_ << std::endl;
    getSignalMCAnaVtxZ_h(signalMCAnaFilename_, signal_pdgid_); 

    //Read signal ana vertex tuple, and convert to mutable tuple
    std::cout << "[SimpZBiOptimization]::Reading Signal AnaVertex Tuple from file " 
        << signalVtxAnaFilename_.c_str() << std::endl;
    TFile* signalVtxAnaFile = new TFile(signalVtxAnaFilename_.c_str(),"READ");
    signalMTT_ = new MutableTTree(signalVtxAnaFile, signalVtxAnaTreename_);
    signalMTT_->defineMassWindow(lowMass_, highMass_);

    //Read background ana vertex tuple, and convert to mutable tuple
    std::cout << "[SimpZBiOptimization]::Reading Background AnaVertex Tuple from file " 
        << signalVtxAnaFilename_.c_str() << std::endl;
    TFile* bkgVtxAnaFile = new TFile(bkgVtxAnaFilename_.c_str(),"READ");
    bkgMTT_ = new MutableTTree(bkgVtxAnaFile, bkgVtxAnaTreename_);
    bkgMTT_->defineMassWindow(lowMass_, highMass_);

    //Add new variables, as defined in MutableTTree.cxx, from the processor configuration script
    std::cout << "[SimpZBiOptimization]::Adding New Variables to Tuples" << std::endl;
    for(std::vector<std::string>::iterator it = new_variables_.begin(); it != new_variables_.end(); it++){
        int param_idx = std::distance(new_variables_.begin(), it);
        std::cout << "[SimpZBiOptimization]::Attempting to add new variable " << *it << 
            " with parameter " << new_variable_params_.at(param_idx) << std::endl;
        addNewVariables(signalMTT_, *it, new_variable_params_.at(param_idx));
        addNewVariables(bkgMTT_, *it, new_variable_params_.at(param_idx));
    }

    //Apply any variable corrections here
    std::cout << "Shifting background unc_vtx_ele_track_z0 by 0.1 mm" << std::endl;
    bkgMTT_->shiftVariable("unc_vtx_ele_track_z0", 0.1);
    std::cout << "Shifting background unc_vtx_pos_track_z0 by 0.1 mm" << std::endl;
    bkgMTT_->shiftVariable("unc_vtx_pos_track_z0", 0.1);
    
    //Finalize Initialization of New Mutable Tuples
    std::cout << "[SimpZBiOptimization]::Finalizing Initialization of New Mutable Tuples" << std::endl;
    signalMTT_->Fill();
    bkgMTT_->Fill();

    //Initialize Persistent Cut Selector. These cuts are applied to all events.
    //Persistent Cut values are updated each iteration with the value of the best performing Test Cut in
    //that iteration.
    std::cout << "[SimpZBiOptimization]::Initializing Set of Persistent Cuts" << std::endl;
    persistentCutsSelector_ = new IterativeCutSelector("persistentCuts", cuts_cfgFile_);
    persistentCutsSelector_->LoadSelection();
    persistentCutsSelector_->filterCuts(cutVariables_);
    persistentCutsPtr_ = persistentCutsSelector_->getPointerToCuts();
    std::cout << "Persistent Cuts: " << std::endl;
    persistentCutsSelector_->printCuts();

    //initalize Test Cuts
    std::cout << "[SimpZBiOptimization]::Initializing Set of Test Cuts" << std::endl;
    testCutsSelector_ = new IterativeCutSelector("testCuts",cuts_cfgFile_);
    testCutsSelector_->LoadSelection();
    testCutsSelector_->filterCuts(cutVariables_);
    testCutsPtr_ = testCutsSelector_->getPointerToCuts();
    std::cout << "Test Cuts: " << std::endl;
    testCutsSelector_->printCuts();

    //Initialize signal histograms
    std::cout << "[SimpZBiOptimization]::Initializing Signal Variable Histograms" << std::endl;
    signalHistos_= std::make_shared<ZBiHistos>("signal");
    signalHistos_->debugMode(debug_);
    signalHistos_->loadHistoConfig(variableHistCfgFilename_);
    signalHistos_->DefineHistos();

    ////Initialize background histograms
    std::cout << "[SimpZBiOptimization]::Initializing Background Variable Histograms" << std::endl;
    bkgHistos_ = std::make_shared<ZBiHistos>("background");
    bkgHistos_->debugMode(debug_);
    bkgHistos_->loadHistoConfig(variableHistCfgFilename_);
    bkgHistos_->DefineHistos();
    
    //Initialize Test Cut histograms
    std::cout << "[SimpZBiOptimization]::Initializing Test Cut Variable Histograms" << std::endl;
    testCutHistos_ = std::make_shared<ZBiHistos>("testCutHistos");
    testCutHistos_->debugMode(debug_);
    testCutHistos_->DefineHistos();

    //Add Test Cut Analysis Histograms necessary for calculating background and signal
    std::cout << "[SimpZBiOptimization]::Initializing Test Cut Analysis Histograms" << std::endl;
    for(cut_iter_ it=testCutsPtr_->begin(); it!=testCutsPtr_->end(); it++){
        std::string name = it->first;
        //Used to select true z vertex distribution given a cut in unc_vtx_z
        testCutHistos_->addHisto2d("unc_vtx_z_vs_true_vtx_z_"+name+"_hh","unc z_{vtx} [mm]",
            1500, -50.0, 100.0,"true z_{vtx} [mm]",200,-50.3,149.7);
        //signalSelZ models signal selection efficiency
        testCutHistos_->addHisto1d("signal_SelZ_"+name+"_h","true z_{vtx} [mm]",
                200, -50.3, 149.7);
        //background_zVtx provides basis for background model, used to estimate nbackground in signal region
        testCutHistos_->addHisto1d("background_zVtx_"+name+"_h","unc z_{vtx} [mm]",
                150, -50.0, 100.0);
    }

    //Initialize processor histograms that summarize iterative results
    std::cout << "[SimpZBiOptimization]::Initializing Itearitve Result Histograms" << std::endl;
    processorHistos_ = std::make_shared<ZBiHistos>("zbi_processor");
    processorHistos_->defineZBiCutflowProcessorHistograms();

    //Fill Initial Signal histograms
    std::cout << "[SimpZBiOptimization]::Filling initial signal histograms" << std::endl;
    for(int e=0; e < signalMTT_->GetEntries(); e++){
        signalMTT_->GetEntry(e);
        if(testSpecialCut_){
            if(!signalMTT_->testImpactParameterCut())
                continue;
        }
        fillEventHistograms(signalHistos_, signalMTT_);
    }

    std::cout << "[SimpZBiOptimization]::Filling initial background histograms" << std::endl;
    //Fill Initial Background Histograms
    for(int e=0; e < bkgMTT_->GetEntries(); e++){
        bkgMTT_->GetEntry(e);
        if(testSpecialCut_){
            if(!bkgMTT_->testImpactParameterCut())
                continue;
        }
        fillEventHistograms(bkgHistos_, bkgMTT_);
    }
    
    //Write initial variable histograms for signal and background
    std::cout << "[SimpZBiOptimization]::Writing Initial Histograms" << std::endl;
    signalHistos_->writeHistos(outFile_,"initial_signal");
    bkgHistos_->writeHistos(outFile_,"initial_background");

    //Integrate each signal variable distribution
    //This value is the reference for cutting n% of signal in a given variable

    //Integrate each signal variable distribution.
    //When iterating Test Cuts, we reference these intial integrals, cutting n% of the original distribution.
    std::cout << "[SimpZBiOptimization]::Integrating initial Signal distributions" << std::endl;
    for(cut_iter_ it=testCutsPtr_->begin(); it!=testCutsPtr_->end(); it++){
        std::string cutname = it->first;
        std::string var = persistentCutsSelector_->getCutVar(cutname);
        initialIntegrals_[var] = signalHistos_->integrateHistogram1D("signal_"+var+"_h");
    }

    //Set initial value of each Persistent Cut to where 0% of Signal distribution is cut in that variable
    std::cout << "[SimpZBiOptimization]::Set initial Persistent Cut values based on cutting 0% Signal" << std::endl;
    for(cut_iter_ it=persistentCutsPtr_->begin(); it!=persistentCutsPtr_->end(); it++){
        std::string cutname = it->first;
        std::string cutvar = persistentCutsSelector_->getCutVar(cutname);
        bool isCutGT = persistentCutsSelector_->isCutGreaterThan(cutname);
        double cutvalue = signalHistos_->cutFractionOfSignalVariable(cutvar, isCutGT, 0.0, initialIntegrals_[cutvar]);
        persistentCutsSelector_->setCutValue(cutname, cutvalue);
        std::cout << "[SimpZBiOptimization]::Persistent Cut "<< cutname << ": " << 
            persistentCutsSelector_->getCut(cutname) << " cuts 0% of Signal in variable " << cutvar << std::endl;
    }
}


bool SimpZBiOptimizationProcessor::process(){
    std::cout << "[SimpZBiOptimizationProcessor] process()" << std::endl;

    //step_size defines n% of signal distribution to cut in a given variable
    double cutFraction = step_size_;
    if(max_iteration_ > (int)1.0/cutFraction)
        max_iteration_ = (int)1.0/cutFraction;

    //Iteratively cut n% of the signal distribution for a given Test Cut variable
    for(int iteration = 0; iteration < max_iteration_; iteration ++){
        double cutSignal = (double)iteration*step_size_*100.0;
        cutSignal = round(cutSignal);
        if(debug_) std::cout << "## ITERATION " << iteration << " ##" << std::endl;

        //Reset histograms at the start of each iteration
        //Histograms will change with each iteration. 
        if(debug_) std::cout << "[SimpZBiOptimization]::Resetting Histograms for Iteration" << std::endl;
        signalHistos_->resetHistograms1d();
        signalHistos_->resetHistograms2d();
        bkgHistos_->resetHistograms1d();
        bkgHistos_->resetHistograms2d();
        testCutHistos_->resetHistograms1d();
        testCutHistos_->resetHistograms2d();

        //At the start of each iteration, save the persistent cut values
        for(cut_iter_ it=persistentCutsPtr_->begin(); it!=persistentCutsPtr_->end(); it++){
            std::string cutname = it->first;
            double cutvalue = persistentCutsSelector_->getCut(cutname);
            int cutid = persistentCutsSelector_->getCutID(cutname);
            processorHistos_->Fill2DHisto("persistent_cuts_hh",(double)cutSignal, 
                    (double)cutid,cutvalue);
            processorHistos_->set2DHistoYlabel("persistent_cuts_hh",cutid,cutname);
        }

        //Fill signal variable distributions
        for(int e=0;  e < signalMTT_->GetEntries(); e++){
            signalMTT_->GetEntry(e);

            //Apply current set of persistent cuts to all events
            if(failPersistentCuts(signalMTT_))
                continue;

            //Fill Signal variable distributions
            fillEventHistograms(signalHistos_, signalMTT_);
        }
        
        //Loop over each Test Cut. Cut n% of signal distribution in Test Cut variable
        for(cut_iter_ it=testCutsPtr_->begin(); it!=testCutsPtr_->end(); it++){
            std::string cutname = it->first;
            std::string cutvar = testCutsSelector_->getCutVar(cutname);
            
            bool isCutGT = testCutsSelector_->isCutGreaterThan(cutname);
            double cutvalue = signalHistos_->cutFractionOfSignalVariable(cutvar, isCutGT, 
                    iteration*cutFraction, initialIntegrals_[cutvar]);
            testCutsSelector_->setCutValue(cutname, cutvalue);
            if(debug_){
                std::cout << "[SimpZBiOptimization]::Test Cut " << cutname << std::endl; 
                std::cout << "Cut value " << testCutsSelector_->getCut(cutname) << " cuts " << cutSignal 
                    << "% of signal distribution in this variable." <<  std::endl;
            }
        }

        //Fill Background Histograms corresponding to each Test Cut
        for(int e=0;  e < bkgMTT_->GetEntries(); e++){
            bkgMTT_->GetEntry(e);

            //Apply persistent cuts
            if(failPersistentCuts(bkgMTT_))
                continue;

            fillEventHistograms(bkgHistos_, bkgMTT_);

            //Loop over each Test Cut
            for(cut_iter_ it=testCutsPtr_->begin(); it!=testCutsPtr_->end(); it++){
               std::string cutname = it->first;
               std::string cutvar = testCutsSelector_->getCutVar(cutname);

               //apply Test Cut
               if(failTestCut(cutname, bkgMTT_))
                   continue;

                //If event passes Test Cut, fill vertex z distribution. 
                //This distribution is used to build the Background Model corresponding to each Test Cut
                testCutHistos_->Fill1DHisto("background_zVtx_"+cutname+"_h",
                        bkgMTT_->getValue("unc_vtx_z"),1.0);
            }
        }

        //For each Test Cut, build relationship between Signal truth z_vtx, and reconstructed z_vtx
        //This is used to get the truth Signal Selection Efficiency F(z), given a Zcut in reconstructed z_vtx
        for(int e=0;  e < signalMTT_->GetEntries(); e++){
            signalMTT_->GetEntry(e);

            //Apply persistent cuts
            if(failPersistentCuts(signalMTT_))
                continue;

            //Loop over each Test Cut and plot unc_vtx_z vs true_vtx_z
            for(cut_iter_ it=testCutsPtr_->begin(); it!=testCutsPtr_->end(); it++){
                std::string cutname = it->first;
                std::string cutvar = testCutsSelector_->getCutVar(cutname);
                double cutvalue = testCutsSelector_->getCut(cutname);
                //Apply Test Cut
                if(!testCutsSelector_->passCutGTorLT(cutname, signalMTT_->getValue(cutvar)))
                    continue;
                testCutHistos_->Fill2DHisto("unc_vtx_z_vs_true_vtx_z_"+cutname+"_hh",
                        signalMTT_->getValue("unc_vtx_z"),signalMTT_->getValue("true_vtx_z"),1.0);
            }
        }

        //Calcuate the Binomial Significance ZBi corresponding to each Test Cut
        //Test Cut with maximum ZBi after cutting n% of signal distribution in a given variable is selected
        //Test Cut value is added to list of Persistent Cuts, and is applied to all events in following iterations.
        double best_zbi = -9999.9;
        std::string best_cutname;
        double best_cutvalue;

        //Loop over Test Cuts
        for(cut_iter_ it=testCutsPtr_->begin(); it!=testCutsPtr_->end(); it++){
            std::string cutname = it->first;
            double cutvalue = testCutsSelector_->getCut(cutname);
            int cutid = testCutsSelector_->getCutID(cutname);
            std::cout << "[SimpZBiOptimization]::Calculating ZBi for Test Cut " << cutname << std::endl;
            std::cout << "Test Cut ID: " << cutid << " | Test Cut Value: " << cutvalue << std::endl;

            //Build Background Model, used to estimate nbkg in Signal Region
            std::cout << "[SimpZBiOptimization]::Building Background Model to estimate nbkg in Signal Region" 
                << std::endl;
            TF1* bkg_model = (TF1*)testCutHistos_->fitExponentialTail("background_zVtx_"+cutname, 200.0); 

            //Get signal unc_vtx_z vs true_vtx_z
            TH2F* vtx_z_hh = (TH2F*)testCutHistos_->get2dHisto("testCutHistos_unc_vtx_z_vs_true_vtx_z_"+cutname+"_hh");

            //CD to output file to save resulting plots
            outFile_->cd();

            //Graphs track the performance of a Test Cut as a function of Zcut position
            TGraph* zcutscan_zbi_g = new TGraph();
            zcutscan_zbi_g->SetName(("zcut_vs_zbi_"+cutname+"_g").c_str());
            zcutscan_zbi_g->SetTitle(("zcut_vs_zbi_"+cutname+"_g;zcut [mm];zbi").c_str());

            TGraph* zcutscan_nsig_g = new TGraph();
            zcutscan_nsig_g->SetName(("zcut_vs_nsig_"+cutname+"_g").c_str());
            zcutscan_nsig_g->SetTitle(("zcut_vs_nsig_"+cutname+"_g;zcut [mm];nsig").c_str());

            TGraph* zcutscan_nbkg_g = new TGraph();
            zcutscan_nbkg_g->SetName(("zcut_vs_nbkg_"+cutname+"_g").c_str());
            zcutscan_nsig_g->SetTitle(("zcut_vs_nbkg_"+cutname+"_g;zcut [mm];nbkg").c_str());

            TGraph* nbkg_zbi_g = new TGraph();
            nbkg_zbi_g->SetName(("nbkg_vs_zbi_"+cutname+"_g").c_str());
            nbkg_zbi_g->SetTitle(("nbkg_vs_zbi_"+cutname+"_g;nbkg;zbi").c_str());

            TGraph* nsig_zbi_g = new TGraph();
            nsig_zbi_g->SetName(("nsig_vs_zbi_"+cutname+"_g").c_str());
            nsig_zbi_g->SetTitle(("nsig_vs_zbi_"+cutname+"_g;nsig;zbi").c_str());

            TH2F* nsig_zcut_hh = new TH2F(("nsig_v_zcut_zbi_"+cutname+"_hh").c_str(),
                    ("nsig_v_zcut_zbi_"+cutname+"_hh; zcut [mm]; Nbkg").c_str(),
                    200,-50.3,149.7,3000,0.0,300.0);

            TH2F* nbkg_zcut_hh = new TH2F(("nbkg_v_zcut_zbi_"+cutname+"_hh").c_str(),
                    ("nbkg_v_zcut_zbi_"+cutname+"_hh; zcut [mm]; Nbkg").c_str(),
                    200,-50.3,149.7,3000,0.0,300.0);


            //Find maximum position of Zcut --> ZBi calculation requires non-zero background
            //Start the Zcut position at the target
            double max_zcut = -4.0;
            double testIntegral = bkg_model->Integral(max_zcut, bkg_model->GetXmax());
            while(testIntegral > min_ztail_events_){
                max_zcut = max_zcut+0.1;
                testIntegral = bkg_model->Integral(max_zcut, bkg_model->GetXmax());
            }
            std::cout << "[SimpZBiOptimization]::Maximum Zcut: " << max_zcut << 
                " gives " << testIntegral << " background events"  << std::endl;

            //If configuration does not specify scanning Zcut values, use single Zcut position at maximum position.
            double min_zcut = -4.0;
            if(!scan_zcut_)
                min_zcut = max_zcut;
            std::cout << "[SimpZBiOptimization]::Minimum Zcut position: " << min_zcut << std::endl;

            //Get the signal vtx z selection efficiency *before* zcut is applied
            TH1F* true_vtx_NoZ_h = (TH1F*)vtx_z_hh->ProjectionY((cutname+"_"+"true_vtx_z_projy").c_str(),1,vtx_z_hh->GetXaxis()->GetNbins(),"");

            //Scan Zcut position and calculate ZBi
            double best_scan_zbi = -999.9;
            double best_scan_zcut;
            double best_scan_nsig;
            double best_scan_nbkg;
            for(double zcut = min_zcut; zcut < std::round(max_zcut+1.0); zcut = zcut+1.0){
                double Nbkg = bkg_model->Integral(zcut,bkg_model->GetXmax());
                
                //Get the Signal truth vertex z distribution beyond the reconstructed vertex Zcut
                TH1F* true_vtx_z_h = (TH1F*)vtx_z_hh->ProjectionY((std::to_string(zcut)+"_"+cutname+"_"+"true_vtx_z_projy").c_str(),vtx_z_hh->GetXaxis()->FindBin(zcut)+1,vtx_z_hh->GetXaxis()->GetNbins(),"");

                //Convert the truth vertex z distribution beyond Zcut into the appropriately binned Selection.
                //Binning must match Signal pre-trigger distribution, in order to take Efficiency between them. 
                TH1F* signalSelZ_h = 
                    (TH1F*)signalSimZ_h_->Clone(("testCutHistos_signal_SelZ_"+cutname+"_h").c_str());
                for(int i=0; i<201; i++){
                    signalSelZ_h->SetBinContent(i,true_vtx_z_h->GetBinContent(i));
                }

                //Get the Signal Selection Efficiency, as a function of truth vertex Z, F(z)
                TEfficiency* effCalc_h = new TEfficiency(*signalSelZ_h, *signalSimZ_h_);


                double eps2 = std::pow(10, logEps2_);
                double eps = std::sqrt(eps2);

                //Calculate expected signal for Neutral Dark Vector "rho"
                double nSigRho = simpEqs_->expectedSignalCalculation(signal_mass_, eps, true, false, 
                        1.35, effCalc_h, -4.3, zcut);

                //Calculate expected signal for Neutral Dark Vector "phi"
                double nSigPhi = simpEqs_->expectedSignalCalculation(signal_mass_, eps, false, true, 
                        1.35, effCalc_h, -4.3, zcut);

                double Nsig = nSigRho + nSigPhi;

                /*
                //SIMP EXPECTED SIGNAL CALCUATION
                double simpAp_mass_MeV = signal_mass_*(3/1.8);
                double m_pi = simpAp_mass_MeV/3.0;
                double alpha_D = 0.01;
                double m_l = 0.511;
                double f_pi = m_pi/(4.*M_PI);
                //Calculate expected signal for Neutral Dark Vector "rho"
                double nSigRho = simpEqs_->expectedSignalCalculation(simpAp_mass_MeV, m_pi, signal_mass_, eps,
                        alpha_D, f_pi, m_l, true, false, 1.35, effCalc_h, -4.3, zcut);

                //Calculate expected signal for Neutral Dark Vector "phi"
                double nSigPhi = simpEqs_->expectedSignalCalculation(simpAp_mass_MeV, m_pi, signal_mass_, eps,
                        alpha_D, f_pi, m_l, false, true, 1.35, effCalc_h, -4.3, zcut);
                */


                /*
                std::string mesons[2] = {"rho","phi"};
                for(int i =0; i < 2; i++){
                    bool rho = false;
                    bool phi = false;
                    if(mesons[i] == "rho") rho = true;
                    else phi = true;

                    double ctau = simpEqs_->getCtau(simpAp_mass_MeV,m_pi,signal_mass_,eps,alpha_D,f_pi,m_l,rho);
                    double E_V = 1.35; //GeV <-this absolutely needs to be fixed
                    double gcTau = ctau * simpEqs_->gamma(signal_mass_/1000.0,E_V);

                    double effVtx = 0.0;
                    for(int zbin =0; zbin < 201; zbin++){
                        double zz = signalSelZ_h->GetBinLowEdge(zbin);
                        if(zz < zcut) continue;
                        effVtx += (TMath::Exp((-4.3-zz)/gcTau)/gcTau)*
                            (effCalc_h->GetEfficiency(zbin) - 
                             effCalc_h->GetEfficiencyErrorLow(zbin))*
                            signalSelZ_h->GetBinWidth(zbin);
                    }

                    double tot_apProd = (3.*137/2.)*3.14159*(simpAp_mass_MeV*eps2*radFrac_*dNdm_)/
                        radAcc_;

                    double br_Vpi_val = 
                        simpEqs_->br_Vpi(simpAp_mass_MeV,m_pi,signal_mass_,alpha_D,f_pi,rho,phi);
                    
                    double br_V_to_ee = 1.0;
                    Nsig = Nsig + tot_apProd*effVtx*br_V_to_ee*br_Vpi_val;
                }*/


                //CLEAR POINTERS
                delete effCalc_h;

                //Round Nsig, Nbkg, and then ZBi later
                Nsig = round(Nsig);
                Nbkg = round(Nbkg);

                //Calculate ZBi for this Test Cut using this zcut value
                double n_on = Nsig + Nbkg;
                double tau = 1.0;
                double n_off = Nbkg;
                double ZBi = calculateZBi(n_on, n_off, tau);
                ZBi = round(ZBi);

                std::cout << "Zcut = " << zcut << std::endl;
                std::cout << "Nsig = " << Nsig << std::endl;
                std::cout << "n_bkg: " << Nbkg << std::endl;
                std::cout << "n_on: " << n_on << std::endl;
                std::cout << "n_off: " << n_off << std::endl;
                std::cout << "ZBi: " << ZBi << std::endl;

                //Update Test Cut with best scan values
                if(ZBi > best_scan_zbi){
                    best_scan_zbi = ZBi;
                    best_scan_zcut = zcut;
                    best_scan_nsig = Nsig;
                    best_scan_nbkg = Nbkg;
                }
                
                //Fill TGraphs
                zcutscan_zbi_g->SetPoint(zcutscan_zbi_g->GetN(),zcut, ZBi);
                zcutscan_nbkg_g->SetPoint(zcutscan_nbkg_g->GetN(),zcut, Nbkg);
                zcutscan_nsig_g->SetPoint(zcutscan_nsig_g->GetN(),zcut, Nsig);
                nbkg_zbi_g->SetPoint(nbkg_zbi_g->GetN(),Nbkg, ZBi);
                nsig_zbi_g->SetPoint(nsig_zbi_g->GetN(),Nsig, ZBi);

                nsig_zcut_hh->Fill(zcut,Nsig,ZBi);
                nbkg_zcut_hh->Fill(zcut,Nbkg,ZBi);
            }

            //Write graph of zcut vs zbi for the Test Cut
            writeGraph(outFile_, "testCutsPtr_pct_sig_cut_"+std::to_string(cutSignal), 
                    zcutscan_zbi_g);
            writeGraph(outFile_, "testCutsPtr_pct_sig_cut_"+std::to_string(cutSignal), 
                    zcutscan_nbkg_g);
            writeGraph(outFile_, "testCutsPtr_pct_sig_cut_"+std::to_string(cutSignal), 
                    zcutscan_nsig_g);

            //delete TGraph pointers
            delete zcutscan_zbi_g;
            delete zcutscan_nsig_g;
            delete zcutscan_nbkg_g;
            delete nbkg_zbi_g;
            delete nsig_zbi_g;
            delete nsig_zcut_hh;
            delete nbkg_zcut_hh;

           //Fill Summary Histograms Test Cuts at best zcutscan value
           processorHistos_->Fill2DHisto("test_cuts_values_hh",(double)cutSignal, (double)cutid,cutvalue);
           processorHistos_->set2DHistoYlabel("test_cuts_values_hh",cutid,cutname);

           processorHistos_->Fill2DHisto("test_cuts_ZBi_hh",(double)cutSignal, (double)cutid,best_scan_zbi);
           processorHistos_->set2DHistoYlabel("test_cuts_ZBi_hh",cutid,cutname);

           processorHistos_->Fill2DHisto("test_cuts_zcut_hh",(double)cutSignal, (double)cutid,best_scan_zcut);
           processorHistos_->set2DHistoYlabel("test_cuts_zcut_hh",cutid,cutname);

           processorHistos_->Fill2DHisto("test_cuts_nsig_hh",(double)cutSignal, (double)cutid,best_scan_nsig);
           processorHistos_->set2DHistoYlabel("test_cuts_nsig_hh",cutid,cutname);

           processorHistos_->Fill2DHisto("test_cuts_nbkg_hh",(double)cutSignal, (double)cutid,best_scan_nbkg);
           processorHistos_->set2DHistoYlabel("test_cuts_nbkg_hh",cutid,cutname);

            //Check if the best cutscan zbi for this Test Cut is the best overall ZBi for all Test Cuts
            if(best_scan_zbi > best_zbi){
                best_zbi = best_scan_zbi;
                best_cutname = cutname;
                best_cutvalue = cutvalue;
            }

        } //END LOOP OVER TEST CUTS

        //Find the overall Best Test Cut for this iteration. Apply the new best cut to the list of
        //persistent cuts, so that it carries over to the next iteration
        processorHistos_->Fill2DHisto("best_test_cut_ZBi_hh",(double)cutSignal, best_zbi, 
                (double)testCutsSelector_->getCutID(best_cutname));

        if(debug_){
            std::cout << "Iteration " << iteration << " Best Test Cut is " << best_cutname 
                << " "  << best_cutvalue << " with ZBi=" << best_zbi << std::endl;
            std::cout << "Update persistent cuts list with this best test cut..." << std::endl;
            std::cout << "[Persistent Cuts] Before update:" << std::endl;
            persistentCutsSelector_->printCuts();
        }

        //Keep the cut that results in the largest ZBi value and apply that cut
        //to all events in the next iteration
        persistentCutsSelector_->setCutValue(best_cutname, best_cutvalue);
        if(debug_){
            std::cout << "[Persistent Cuts] After update:" << std::endl;
            persistentCutsSelector_->printCuts();
        }

        //Write iteration histos
        signalHistos_->writeHistos(outFile_,"signal_pct_sig_cut_"+std::to_string(cutSignal));
        bkgHistos_->writeHistos(outFile_,"tritrig_pct_sig_cut_"+std::to_string(cutSignal));
        testCutHistos_->writeHistos(outFile_, "testCutsPtr_pct_sig_cut_"+std::to_string(cutSignal));
    }
}

void SimpZBiOptimizationProcessor::printZBiMatrix(){
    typedef std::map<std::string,std::vector<std::pair<double,double>>>::iterator iter;
    for(iter it = global_ZBi_map_.begin(); it != global_ZBi_map_.end(); it++){
        std::cout << '\n';
        std::cout << it->first << ": ";
        for(std::vector<std::pair<double,double>>::iterator vec_it = it->second.begin(); vec_it != it->second.end(); vec_it++){
            double cutvalue = vec_it->first;
            double zbi = vec_it->second;
            std::cout << " |" << cutvalue << "," << zbi << "| ";
        }
        std::cout << '\n';
    }
}

void SimpZBiOptimizationProcessor::finalize() {
    std::cout << "[SimpZBiOptimizationProcessor] finalize()" << std::endl;

    if(debug_){
        std::cout << "FINAL LIST OF PERSISTENT CUTS " << std::endl;
        persistentCutsSelector_->printCuts();
    }

    processorHistos_->saveHistos(outFile_);
    testCutHistos_->writeGraphs(outFile_,"");

    printZBiMatrix();
}

double SimpZBiOptimizationProcessor::calculateZBi(double n_on, double n_off, double tau){
    double P_Bi = TMath::BetaIncomplete(1./(1.+tau),n_on,n_off+1);
    double Z_Bi = std::pow(2,0.5)*TMath::ErfInverse(1-2*P_Bi);
    return Z_Bi;
}

bool SimpZBiOptimizationProcessor::failPersistentCuts(MutableTTree* MTT){
    bool failCuts = false;
    for(cut_iter_ it=persistentCutsPtr_->begin(); it!=persistentCutsPtr_->end(); it++){
        std::string cutname = it->first;
        std::string cutvar = persistentCutsSelector_->getCutVar(cutname);
        //If no value inside the tuple exists for this cut, do not apply the cut.
        if(!MTT->variableExists(cutvar))
            continue;
        if(!persistentCutsSelector_->passCutGTorLT(cutname, MTT->getValue(cutvar))){ 
            failCuts = true;
            break;
        }
    }

    //Testing 2016 impact parameter cut 04/10/2023
    //THIS SHOULD EVENTUALLY BE REMOVED
    if(testSpecialCut_){
        //if(!MTT->impactParameterCut2016Canonical(signal_mass_))
        //    failCuts = true;
        if(!MTT->testImpactParameterCut())
            failCuts = true;
    }

    return failCuts;
}

bool SimpZBiOptimizationProcessor::failTestCut(std::string cutname, MutableTTree* MTT){

    std::string cutvar = testCutsSelector_->getCutVar(cutname);
    double cutvalue = testCutsSelector_->getCut(cutname);
    //If cut variable is not found in the list of tuples, do not apply cut
    if(!MTT->variableExists(cutvar))
        return false;
    if(!testCutsSelector_->passCutGTorLT(cutname, MTT->getValue(cutvar)))
        return true;
    else
        return false;
}

bool SimpZBiOptimizationProcessor::failTestCut(std::string cutname, std::map<std::string,double*> tuple){

    std::string cutvar = testCutsSelector_->getCutVar(cutname);
    double cutvalue = testCutsSelector_->getCut(cutname);
    //If cut variable is not found in the list of tuples, do not apply cut
    if(tuple.find(cutvar) == tuple.end())
        return false;
    if(!testCutsSelector_->passCutGTorLT(cutname, *tuple[cutvar]))
        return true;
    else
        return false;
}

void SimpZBiOptimizationProcessor::getSignalMCAnaVtxZ_h(std::string signalMCAnaFilename, 
        std::string signal_pdgid){
    //Read pre-trigger Signal MCAna vertex z distribution
    signalSimZ_h_ = new TH1F("signal_SimZ_h_","signal_SimZ;true z_{vtx} [mm];events", 200, -50.3, 149.7);
    TFile* signalMCAnaFile = new TFile(signalMCAnaFilename_.c_str(), "READ");
    TH1F* mcAnaSimZ_h = (TH1F*)signalMCAnaFile->Get(("mcAna/mcAna_mc"+signal_pdgid+"Z_h").c_str()); 
    for(int i=0; i < 201; i++){
        signalSimZ_h_->SetBinContent(i,mcAnaSimZ_h->GetBinContent(i));
    }
    signalMCAnaFile->Close();
    delete signalMCAnaFile;
}


void SimpZBiOptimizationProcessor::writeGraph(TFile* outF, std::string folder, TGraph* g){
    if (outF) outF->cd();
    TDirectory* dir{nullptr};
    std::cout<<folder.c_str()<<std::endl;
    if (!folder.empty()) {
        dir = outF->mkdir(folder.c_str(),"",true);
        dir->cd();
    }
    g->Write();
}

double SimpZBiOptimizationProcessor::round(double var){
    float value = (int)(var * 100 + .5);
    return (double)value / 100;
}

DECLARE_PROCESSOR(SimpZBiOptimizationProcessor);
