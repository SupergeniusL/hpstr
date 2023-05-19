#ifndef MUTABLE_TTREE_H
#define MUTABLE_TTREE_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>


#include <TTree.h>
#include <TFile.h>
#include <TBranch.h>

class MutableTTree {

    public:

        MutableTTree(TFile* infile);

        MutableTTree(TFile* infile, std::string tree_name);

        int GetEntries(){return tree_->GetEntries();}

        void GetEntry(int entry){tree_->GetEntry(entry);}

        //void Fill(){tree_->Fill();}

        void Fill();

        //void addVariableZalpha(std::vector<double> impact_parameter_cut);
        void addNewVariable(std::string, double param);
        void addVariableZalpha(double y_intercept, double slope, double alpha_z);
        void addVariableZalpha(double slope);
        void addVariableZbravo();
        void addVariableZbravoAlpha(double slope);
        void addVariableZbravosum();
        void addVariableZbravosumAlpha(double slope);
        void shiftVariable(std::string variable, double shift);
        bool impactParameterCut2016Canonical(double mass);
        bool testImpactParameterCut();
        void addSelectionGreaterThan(std::string selection, double value);

        void shiftZ0(double shift);

        double getValue(std::string branch_name);

        //void addBranch(std::string branch_name);

        void printTree(){tree_->Print();}

        void printEvent();

        void setBranchValue(std::string branch_name, double value){*tuple_[branch_name] = value;}

        void addNewBranch(std::string branch);

        void fillNewBranch(std::string branch, double value);

        void defineMassWindow(double lowMass, double highMass);

        std::vector<std::string> getAllVariables();

        bool variableExists(std::string variable);


        ~MutableTTree();

    protected:
        TTree* tree_{nullptr};
        TTree* newtree_{nullptr};
        std::map<std::string,double*> tuple_;
        std::map<std::string,double*> readTuple_;
        std::map<std::string,double> writeTuple_;



    private:
        void initializeFlatTuple(TTree* tree, std::map<std::string, double*> &tuple_map);
        void copyTTree();
        std::map<std::string,TBranch*> new_branches;
        std::map<std::string, double*> new_variables_;
        std::map<std::string,std::function<double()>> functions_;
        std::map<std::string, double> selectionGT_;
        std::map<std::string,std::function<double()>> variable_shifts_;
        double lowMass_{-999.9};
        double highMass_{-999.9};
};

#endif // __MUTABLE_TTREE_H
