#include "PhysicsTools/FWLite/interface/CommandLineParser.h" 
#include "TFile.h"
#include "TROOT.h"
#include "TLorentzVector.h"
#include "TKey.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include <math.h> 
#include "TMath.h" 
#include <limits>
#include "TSystem.h"

#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "TauAnalysis/ClassicSVfit/interface/ClassicSVfit.h"
#include "TauAnalysis/ClassicSVfit/interface/MeasuredTauLepton.h"
#include "TauAnalysis/ClassicSVfit/interface/svFitHistogramAdapter.h"
#include "TauAnalysis/ClassicSVfit/interface/FastMTT.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"

//If doES       0 does not apply any ES shifts
//              1 applies ES shifts to TT channel, no effect on other channels
//

void copyFiles( optutl::CommandLineParser parser, TFile* fOld, TFile* fNew) ;
void readdir(TDirectory *dir, optutl::CommandLineParser parser, char TreeToUse[], int doES, int doUES, int doRES, int doJES, int year) ;
void CopyFile(const char *fname, optutl::CommandLineParser parser);
void CopyDir(TDirectory *source,optutl::CommandLineParser parser);
void runSVFit(std::vector<classic_svFit::MeasuredTauLepton> & measuredTauLeptons, double measuredMETx, double measuredMETy, TMatrixD &covMET, float num, float &svFitMass, float& svFitPt, float &svFitEta, float &svFitPhi);

int main (int argc, char* argv[]) 
{
   optutl::CommandLineParser parser ("Sets Event Weights in the ntuple");
   parser.addOption("branch",optutl::CommandLineParser::kString,"Branch","__svFit__");
   parser.addOption("newFile",optutl::CommandLineParser::kString,"newFile","newFile.root");
   parser.addOption("inputFile",optutl::CommandLineParser::kString,"input File");
   parser.addOption("newOutputFile",optutl::CommandLineParser::kDouble,"New Output File",0.0);
   parser.addOption("doES",optutl::CommandLineParser::kDouble,"doES",0.0);
   parser.addOption("doUES",optutl::CommandLineParser::kDouble,"doUES",0.0);
   parser.addOption("doRES",optutl::CommandLineParser::kDouble,"doRES",0.0);
   parser.addOption("doJES",optutl::CommandLineParser::kDouble,"doJES",0.0);
   parser.addOption("year",optutl::CommandLineParser::kDouble,"year",0.0);

   parser.parseArguments (argc, argv);

   std::cout << "EXTRA COMMANDS:"
    << "\n --- doES: " << parser.doubleValue("doES") 
    << "\n --- doUES: " << parser.doubleValue("doUES")
    << "\n --- doRES: " << parser.doubleValue("doRES")
    << "\n --- doJES: " << parser.doubleValue("doJES") 
    << "\n --- year: " << parser.doubleValue("year") << std::endl;

   char TreeToUse[80]="first" ;

   TFile *fProduce;//= new TFile(parser.stringValue("newFile").c_str(),"UPDATE");

   if(parser.doubleValue("newOutputFile")>0){
   TFile *f = new TFile(parser.stringValue("inputFile").c_str(),"READ");
     std::cout<<"Creating new outputfile"<<std::endl;
     std::string newFileName = parser.stringValue("newFile");

     fProduce = new TFile(newFileName.c_str(),"RECREATE");
     copyFiles(parser, f, fProduce);//new TFile(parser.stringValue("inputFile").c_str()+"SVFit","UPDATE");
     fProduce = new TFile(newFileName.c_str(),"UPDATE");
     std::cout<<"listing the directories================="<<std::endl;
     fProduce->ls();
     readdir(fProduce,parser,TreeToUse,parser.doubleValue("doES"),parser.doubleValue("doUES"),parser.doubleValue("doRES"),parser.doubleValue("doJES"),parser.doubleValue("year"));

     fProduce->Close();
     f->Close();
   }
   else{
     TFile *f = new TFile(parser.stringValue("inputFile").c_str(),"UPDATE");
     readdir(f,parser,TreeToUse,parser.doubleValue("doES"),parser.doubleValue("doUES"),parser.doubleValue("doRES"),parser.doubleValue("doJES"),parser.doubleValue("year"));
     f->Close();
   }


} 


void readdir(TDirectory *dir, optutl::CommandLineParser parser, char TreeToUse[], int doES, int doUES, int doRES, int doJES, int year) 
{

  TDirectory *dirsav = gDirectory;
  TIter next(dir->GetListOfKeys());
  TKey *key;
  char stringA[80]="first";
  dir->cd();      
  int k=0;
  while ((key = (TKey*)next())) {
    printf("Found key=%s \n",key->GetName());

    TObject *obj = key->ReadObj();
    if (obj->IsA()->InheritsFrom(TDirectory::Class())) {
      dir->cd(key->GetName());
      TDirectory *subdir = gDirectory;
      sprintf(TreeToUse,"%s",key->GetName());
      if (std::string(key->GetName()).find("_tree")){ readdir(subdir,parser,TreeToUse,parser.doubleValue("doES"),parser.doubleValue("doUES"),parser.doubleValue("doRES"),parser.doubleValue("doJES"),parser.doubleValue("year"));
      }

      dirsav->cd();
    }
    else if(k<1 && obj->IsA()->InheritsFrom(TTree::Class())) {
	k++;
	std::cout<<"ici!!!!"<<std::endl;

   TFile ftes2016("TauES_dm_DeepTau2017v2p1VSjet_2016Legacy.root");
   TH1F* hist_lowpt_2016 = (TH1F*) ftes2016.Get("tes");
   TFile ftes2017("TauES_dm_DeepTau2017v2p1VSjet_2017ReReco.root");
   TH1F* hist_lowpt_2017 = (TH1F*) ftes2017.Get("tes");
   TFile ftes2018("TauES_dm_DeepTau2017v2p1VSjet_2018ReReco.root");
   TH1F* hist_lowpt_2018 = (TH1F*) ftes2018.Get("tes");

   TFile ftes2016_high("TauES_dm_DeepTau2017v2p1VSjet_2016Legacy_ptgt100.root");
   TH1F* hist_highpt_2016 = (TH1F*) ftes2016_high.Get("tes");
   TFile ftes2017_high("TauES_dm_DeepTau2017v2p1VSjet_2017ReReco_ptgt100.root");
   TH1F* hist_highpt_2017 = (TH1F*) ftes2017_high.Get("tes");
   TFile ftes2018_high("TauES_dm_DeepTau2017v2p1VSjet_2018ReReco_ptgt100.root");
   TH1F* hist_highpt_2018 = (TH1F*) ftes2018_high.Get("tes");

   TFile ffes2016("TauFES_eta-dm_DeepTau2017v2p1VSe_2016Legacy.root");
   TFile ffes2017("TauFES_eta-dm_DeepTau2017v2p1VSe_2017ReReco.root");
   TFile ffes2018("TauFES_eta-dm_DeepTau2017v2p1VSe_2018ReReco.root");
   TGraph* gfes_2016=(TGraph*) ffes2016.Get("fes");
   TGraph* gfes_2017=(TGraph*) ffes2017.Get("fes");
   TGraph* gfes_2018=(TGraph*) ffes2018.Get("fes");

      TTree *t = (TTree*)obj;
      float svFitMass = -10;
      float svFitPt = -10;
      float svFitEta = -10;
      float svFitPhi = -10;
      float svFitMass_UP = -10;
      float svFitPt_UP = -10;
      float svFitEta_UP = -10;
      float svFitPhi_UP = -10;
      float svFitMass_DOWN = -10;
      float svFitPt_DOWN = -10;
      float svFitEta_DOWN = -10;
      float svFitPhi_DOWN = -10;
      float svFitMass_ESCALEUP = -10;
      float svFitPt_ESCALEUP = -10;
      float svFitEta_ESCALEUP = -10;
      float svFitPhi_ESCALEUP = -10;
      float svFitMass_ESCALEDOWN = -10;
      float svFitPt_ESCALEDOWN = -10;
      float svFitEta_ESCALEDOWN = -10;
      float svFitPhi_ESCALEDOWN = -10;
      float svFitMass_UESUp = -10;
      float svFitPt_UESUp = -10;
      float svFitEta_UESUp = -10;
      float svFitPhi_UESUp = -10;
      float svFitMass_UESDown = -10;
      float svFitPt_UESDown = -10;
      float svFitEta_UESDown = -10;
      float svFitPhi_UESDown = -10;

      float svFitMass_ResolutionUp = -10;
      float svFitPt_ResolutionUp = -10;
      float svFitEta_ResolutionUp = -10;
      float svFitPhi_ResolutionUp = -10;
      float svFitMass_ResolutionDown = -10;
      float svFitPt_ResolutionDown = -10;
      float svFitEta_ResolutionDown = -10;
      float svFitPhi_ResolutionDown = -10;
      float svFitMass_ResponseUp = -10;
      float svFitPt_ResponseUp = -10;
      float svFitEta_ResponseUp = -10;
      float svFitPhi_ResponseUp = -10;
      float svFitMass_ResponseDown = -10;
      float svFitPt_ResponseDown = -10;
      float svFitEta_ResponseDown = -10;
      float svFitPhi_ResponseDown = -10;

      float svFitMass_JERUp = -10;
      float svFitPt_JERUp = -10;
      float svFitEta_JERUp = -10;
      float svFitPhi_JERUp = -10;
      float svFitMass_JERDown = -10;
      float svFitPt_JERDown = -10;
      float svFitEta_JERDown = -10;
      float svFitPhi_JERDown = -10;

      float svFitMass_JetAbsoluteUp = -10;
      float svFitPt_JetAbsoluteUp = -10;
      float svFitEta_JetAbsoluteUp = -10;
      float svFitPhi_JetAbsoluteUp = -10;
      float svFitMass_JetAbsoluteDown = -10;
      float svFitPt_JetAbsoluteDown = -10;
      float svFitEta_JetAbsoluteDown = -10;
      float svFitPhi_JetAbsoluteDown = -10;

      float svFitMass_JetAbsoluteyearUp = -10;
      float svFitPt_JetAbsoluteyearUp = -10;
      float svFitEta_JetAbsoluteyearUp = -10;
      float svFitPhi_JetAbsoluteyearUp = -10;
      float svFitMass_JetAbsoluteyearDown = -10;
      float svFitPt_JetAbsoluteyearDown = -10;
      float svFitEta_JetAbsoluteyearDown = -10;
      float svFitPhi_JetAbsoluteyearDown = -10;

      float svFitMass_JetBBEC1Up = -10;
      float svFitPt_JetBBEC1Up = -10;
      float svFitEta_JetBBEC1Up = -10;
      float svFitPhi_JetBBEC1Up = -10;
      float svFitMass_JetBBEC1Down = -10;
      float svFitPt_JetBBEC1Down = -10;
      float svFitEta_JetBBEC1Down = -10;
      float svFitPhi_JetBBEC1Down = -10;

      float svFitMass_JetBBEC1yearUp = -10;
      float svFitPt_JetBBEC1yearUp = -10;
      float svFitEta_JetBBEC1yearUp = -10;
      float svFitPhi_JetBBEC1yearUp = -10;
      float svFitMass_JetBBEC1yearDown = -10;
      float svFitPt_JetBBEC1yearDown = -10;
      float svFitEta_JetBBEC1yearDown = -10;
      float svFitPhi_JetBBEC1yearDown = -10;

      float svFitMass_JetEC2Up = -10;
      float svFitPt_JetEC2Up = -10;
      float svFitEta_JetEC2Up = -10;
      float svFitPhi_JetEC2Up = -10;
      float svFitMass_JetEC2Down = -10;
      float svFitPt_JetEC2Down = -10;
      float svFitEta_JetEC2Down = -10;
      float svFitPhi_JetEC2Down = -10;

      float svFitMass_JetEC2yearUp = -10;
      float svFitPt_JetEC2yearUp = -10;
      float svFitEta_JetEC2yearUp = -10;
      float svFitPhi_JetEC2yearUp = -10;
      float svFitMass_JetEC2yearDown = -10;
      float svFitPt_JetEC2yearDown = -10;
      float svFitEta_JetEC2yearDown = -10;
      float svFitPhi_JetEC2yearDown = -10;

      float svFitMass_JetFlavorQCDUp = -10;
      float svFitPt_JetFlavorQCDUp = -10;
      float svFitEta_JetFlavorQCDUp = -10;
      float svFitPhi_JetFlavorQCDUp = -10;
      float svFitMass_JetFlavorQCDDown = -10;
      float svFitPt_JetFlavorQCDDown = -10;
      float svFitEta_JetFlavorQCDDown = -10;
      float svFitPhi_JetFlavorQCDDown = -10;

      float svFitMass_JetHFUp = -10;
      float svFitPt_JetHFUp = -10;
      float svFitEta_JetHFUp = -10;
      float svFitPhi_JetHFUp = -10;
      float svFitMass_JetHFDown = -10;
      float svFitPt_JetHFDown = -10;
      float svFitEta_JetHFDown = -10;
      float svFitPhi_JetHFDown = -10;

      float svFitMass_JetHFyearUp = -10;
      float svFitPt_JetHFyearUp = -10;
      float svFitEta_JetHFyearUp = -10;
      float svFitPhi_JetHFyearUp = -10;
      float svFitMass_JetHFyearDown = -10;
      float svFitPt_JetHFyearDown = -10;
      float svFitEta_JetHFyearDown = -10;
      float svFitPhi_JetHFyearDown = -10;

      float svFitMass_JetRelativeBalUp = -10;
      float svFitPt_JetRelativeBalUp = -10;
      float svFitEta_JetRelativeBalUp = -10;
      float svFitPhi_JetRelativeBalUp = -10;
      float svFitMass_JetRelativeBalDown = -10;
      float svFitPt_JetRelativeBalDown = -10;
      float svFitEta_JetRelativeBalDown = -10;
      float svFitPhi_JetRelativeBalDown = -10;

      float svFitMass_JetRelativeSampleUp = -10;
      float svFitPt_JetRelativeSampleUp = -10;
      float svFitEta_JetRelativeSampleUp = -10;
      float svFitPhi_JetRelativeSampleUp = -10;
      float svFitMass_JetRelativeSampleDown = -10;
      float svFitPt_JetRelativeSampleDown = -10;
      float svFitEta_JetRelativeSampleDown = -10;
      float svFitPhi_JetRelativeSampleDown = -10;


      TBranch *newBranch1 = t->Branch("m_sv", &svFitMass, "m_sv/F");
      TBranch *newBranch1D = t->Branch("m_sv_DOWN", &svFitMass_DOWN, "m_sv_DOWN/F");
      TBranch *newBranch1U = t->Branch("m_sv_UP", &svFitMass_UP, "m_sv_UP/F");
      TBranch *newBranch1ESCD = t->Branch("m_sv_ESCALEDOWN", &svFitMass_ESCALEDOWN, "m_sv_ESCALEDOWN/F");
      TBranch *newBranch1ESCU = t->Branch("m_sv_ESCALEUP", &svFitMass_ESCALEUP, "m_sv_ESCALEUP/F");
      TBranch *newBranch1UU = t->Branch("m_sv_UESUp", &svFitMass_UESUp, "m_sv_UESUp/F");
      TBranch *newBranch1UD = t->Branch("m_sv_UESDown", &svFitMass_UESDown, "m_sv_UESDown/F");
      TBranch *newBranch1JetAbsoluteU = t->Branch("m_sv_JetAbsoluteUp", &svFitMass_JetAbsoluteUp, "m_sv_JetAbsoluteUp/F");
      TBranch *newBranch1JetAbsoluteD = t->Branch("m_sv_JetAbsoluteDown", &svFitMass_JetAbsoluteDown, "m_sv_JetAbsoluteDown/F");
      TBranch *newBranch1JetAbsoluteyearU = t->Branch("m_sv_JetAbsoluteyearUp", &svFitMass_JetAbsoluteyearUp, "m_sv_JetAbsoluteyearUp/F");
      TBranch *newBranch1JetAbsoluteyearD = t->Branch("m_sv_JetAbsoluteyearDown", &svFitMass_JetAbsoluteyearDown, "m_sv_JetAbsoluteyearDown/F");
      TBranch *newBranch1JetBBEC1U = t->Branch("m_sv_JetBBEC1Up", &svFitMass_JetBBEC1Up, "m_sv_JetBBEC1Up/F");
      TBranch *newBranch1JetBBEC1D = t->Branch("m_sv_JetBBEC1Down", &svFitMass_JetBBEC1Down, "m_sv_JetBBEC1Down/F");
      TBranch *newBranch1JetBBEC1yearU = t->Branch("m_sv_JetBBEC1yearUp", &svFitMass_JetBBEC1yearUp, "m_sv_JetBBEC1yearUp/F");
      TBranch *newBranch1JetBBEC1yearD = t->Branch("m_sv_JetBBEC1yearDown", &svFitMass_JetBBEC1yearDown, "m_sv_JetBBEC1yearDown/F");
      TBranch *newBranch1JetEC2U = t->Branch("m_sv_JetEC2Up", &svFitMass_JetEC2Up, "m_sv_JetEC2Up/F");
      TBranch *newBranch1JetEC2D = t->Branch("m_sv_JetEC2Down", &svFitMass_JetEC2Down, "m_sv_JetEC2Down/F");
      TBranch *newBranch1JetEC2yearU = t->Branch("m_sv_JetEC2yearUp", &svFitMass_JetEC2yearUp, "m_sv_JetEC2yearUp/F");
      TBranch *newBranch1JetEC2yearD = t->Branch("m_sv_JetEC2yearDown", &svFitMass_JetEC2yearDown, "m_sv_JetEC2yearDown/F");
      TBranch *newBranch1JetFlavorQCDU = t->Branch("m_sv_JetFlavorQCDUp", &svFitMass_JetFlavorQCDUp, "m_sv_JetFlavorQCDUp/F");
      TBranch *newBranch1JetFlavorQCDD = t->Branch("m_sv_JetFlavorQCDDown", &svFitMass_JetFlavorQCDDown, "m_sv_JetFlavorQCDDown/F");
      TBranch *newBranch1JetHFU = t->Branch("m_sv_JetHFUp", &svFitMass_JetHFUp, "m_sv_JetHFUp/F");
      TBranch *newBranch1JetHFD = t->Branch("m_sv_JetHFDown", &svFitMass_JetHFDown, "m_sv_JetHFDown/F");
      TBranch *newBranch1JetHFyearU = t->Branch("m_sv_JetHFyearUp", &svFitMass_JetHFyearUp, "m_sv_JetHFyearUp/F");
      TBranch *newBranch1JetHFyearD = t->Branch("m_sv_JetHFyearDown", &svFitMass_JetHFyearDown, "m_sv_JetHFyearDown/F");
      TBranch *newBranch1JetRelativeBalU = t->Branch("m_sv_JetRelativeBalUp", &svFitMass_JetRelativeBalUp, "m_sv_JetRelativeBalUp/F");
      TBranch *newBranch1JetRelativeBalD = t->Branch("m_sv_JetRelativeBalDown", &svFitMass_JetRelativeBalDown, "m_sv_JetRelativeBalDown/F");
      TBranch *newBranch1JetRelativeSampleU = t->Branch("m_sv_JetRelativeSampleUp", &svFitMass_JetRelativeSampleUp, "m_sv_JetRelativeSampleUp/F");
      TBranch *newBranch1JetRelativeSampleD = t->Branch("m_sv_JetRelativeSampleDown", &svFitMass_JetRelativeSampleDown, "m_sv_JetRelativeSampleDown/F");
      TBranch *newBranch1JERU = t->Branch("m_sv_JERUp", &svFitMass_JERUp, "m_sv_JERUp/F");
      TBranch *newBranch1JERD = t->Branch("m_sv_JERDown", &svFitMass_JERDown, "m_sv_JERDown/F");
      TBranch *newBranch1ResponseU = t->Branch("m_sv_ResponseUp", &svFitMass_ResponseUp, "m_sv_ResponseUp/F");
      TBranch *newBranch1ResponseD = t->Branch("m_sv_ResponseDown", &svFitMass_ResponseDown, "m_sv_ResponseDown/F");
      TBranch *newBranch1ResolutionU = t->Branch("m_sv_ResolutionUp", &svFitMass_ResolutionUp, "m_sv_ResolutionUp/F");
      TBranch *newBranch1ResolutionD = t->Branch("m_sv_ResolutionDown", &svFitMass_ResolutionDown, "m_sv_ResolutionDown/F");

      TBranch *newBranchPt1 = t->Branch("pt_sv", &svFitPt, "pt_sv/F");
      TBranch *newBranchPt1D = t->Branch("pt_sv_DOWN", &svFitPt_DOWN, "pt_sv_DOWN/F");
      TBranch *newBranchPt1U = t->Branch("pt_sv_UP", &svFitPt_UP, "pt_sv_UP/F");
      TBranch *newBranchPt1ESCD = t->Branch("pt_sv_ESCALEDOWN", &svFitPt_ESCALEDOWN, "pt_sv_ESCALEDOWN/F");
      TBranch *newBranchPt1ESCU = t->Branch("pt_sv_ESCALEUP", &svFitPt_ESCALEUP, "pt_sv_ESCALEUP/F");
      TBranch *newBranchPt1UU = t->Branch("pt_sv_UESUp", &svFitPt_UESUp, "pt_sv_UESUp/F");
      TBranch *newBranchPt1UD = t->Branch("pt_sv_UESDown", &svFitPt_UESDown, "pt_sv_UESDown/F");
      TBranch *newBranchPt1JetAbsoluteU = t->Branch("pt_sv_JetAbsoluteUp", &svFitPt_JetAbsoluteUp, "pt_sv_JetAbsoluteUp/F");
      TBranch *newBranchPt1JetAbsoluteD = t->Branch("pt_sv_JetAbsoluteDown", &svFitPt_JetAbsoluteDown, "pt_sv_JetAbsoluteDown/F");
      TBranch *newBranchPt1JetAbsoluteyearU = t->Branch("pt_sv_JetAbsoluteyearUp", &svFitPt_JetAbsoluteyearUp, "pt_sv_JetAbsoluteyearUp/F");
      TBranch *newBranchPt1JetAbsoluteyearD = t->Branch("pt_sv_JetAbsoluteyearDown", &svFitPt_JetAbsoluteyearDown, "pt_sv_JetAbsoluteyearDown/F");
      TBranch *newBranchPt1JetBBEC1U = t->Branch("pt_sv_JetBBEC1Up", &svFitPt_JetBBEC1Up, "pt_sv_JetBBEC1Up/F");
      TBranch *newBranchPt1JetBBEC1D = t->Branch("pt_sv_JetBBEC1Down", &svFitPt_JetBBEC1Down, "pt_sv_JetBBEC1Down/F");
      TBranch *newBranchPt1JetBBEC1yearU = t->Branch("pt_sv_JetBBEC1yearUp", &svFitPt_JetBBEC1yearUp, "pt_sv_JetBBEC1yearUp/F");
      TBranch *newBranchPt1JetBBEC1yearD = t->Branch("pt_sv_JetBBEC1yearDown", &svFitPt_JetBBEC1yearDown, "pt_sv_JetBBEC1yearDown/F");
      TBranch *newBranchPt1JetEC2U = t->Branch("pt_sv_JetEC2Up", &svFitPt_JetEC2Up, "pt_sv_JetEC2Up/F");
      TBranch *newBranchPt1JetEC2D = t->Branch("pt_sv_JetEC2Down", &svFitPt_JetEC2Down, "pt_sv_JetEC2Down/F");
      TBranch *newBranchPt1JetEC2yearU = t->Branch("pt_sv_JetEC2yearUp", &svFitPt_JetEC2yearUp, "pt_sv_JetEC2yearUp/F");
      TBranch *newBranchPt1JetEC2yearD = t->Branch("pt_sv_JetEC2yearDown", &svFitPt_JetEC2yearDown, "pt_sv_JetEC2yearDown/F");
      TBranch *newBranchPt1JetFlavorQCDU = t->Branch("pt_sv_JetFlavorQCDUp", &svFitPt_JetFlavorQCDUp, "pt_sv_JetFlavorQCDUp/F");
      TBranch *newBranchPt1JetFlavorQCDD = t->Branch("pt_sv_JetFlavorQCDDown", &svFitPt_JetFlavorQCDDown, "pt_sv_JetFlavorQCDDown/F");
      TBranch *newBranchPt1JetHFU = t->Branch("pt_sv_JetHFUp", &svFitPt_JetHFUp, "pt_sv_JetHFUp/F");
      TBranch *newBranchPt1JetHFD = t->Branch("pt_sv_JetHFDown", &svFitPt_JetHFDown, "pt_sv_JetHFDown/F");
      TBranch *newBranchPt1JetHFyearU = t->Branch("pt_sv_JetHFyearUp", &svFitPt_JetHFyearUp, "pt_sv_JetHFyearUp/F");
      TBranch *newBranchPt1JetHFyearD = t->Branch("pt_sv_JetHFyearDown", &svFitPt_JetHFyearDown, "pt_sv_JetHFyearDown/F");
      TBranch *newBranchPt1JetRelativeBalU = t->Branch("pt_sv_JetRelativeBalUp", &svFitPt_JetRelativeBalUp, "pt_sv_JetRelativeBalUp/F");
      TBranch *newBranchPt1JetRelativeBalD = t->Branch("pt_sv_JetRelativeBalDown", &svFitPt_JetRelativeBalDown, "pt_sv_JetRelativeBalDown/F");
      TBranch *newBranchPt1JetRelativeSampleU = t->Branch("pt_sv_JetRelativeSampleUp", &svFitPt_JetRelativeSampleUp, "pt_sv_JetRelativeSampleUp/F");
      TBranch *newBranchPt1JetRelativeSampleD = t->Branch("pt_sv_JetRelativeSampleDown", &svFitPt_JetRelativeSampleDown, "pt_sv_JetRelativeSampleDown/F");
      TBranch *newBranchPt1JERU = t->Branch("pt_sv_JERUp", &svFitPt_JERUp, "pt_sv_JERUp/F");
      TBranch *newBranchPt1JERD = t->Branch("pt_sv_JERDown", &svFitPt_JERDown, "pt_sv_JERDown/F");
      TBranch *newBranchPt1ResponseU = t->Branch("pt_sv_ResponseUp", &svFitPt_ResponseUp, "pt_sv_ResponseUp/F");
      TBranch *newBranchPt1ResponseD = t->Branch("pt_sv_ResponseDown", &svFitPt_ResponseDown, "pt_sv_ResponseDown/F");
      TBranch *newBranchPt1ResolutionU = t->Branch("pt_sv_ResolutionUp", &svFitPt_ResolutionUp, "pt_sv_ResolutionUp/F");
      TBranch *newBranchPt1ResolutionD = t->Branch("pt_sv_ResolutionDown", &svFitPt_ResolutionDown, "pt_sv_ResolutionDown/F");

      TBranch *newBranchEta1 = t->Branch("eta_sv", &svFitEta, "eta_sv/F");
      TBranch *newBranchEta1D = t->Branch("eta_sv_DOWN", &svFitEta_DOWN, "eta_sv_DOWN/F");
      TBranch *newBranchEta1U = t->Branch("eta_sv_UP", &svFitEta_UP, "eta_sv_UP/F");
      TBranch *newBranchEta1ESCD = t->Branch("eta_sv_ESCALEDOWN", &svFitEta_ESCALEDOWN, "eta_sv_ESCALEDOWN/F");
      TBranch *newBranchEta1ESCU = t->Branch("eta_sv_ESCALEUP", &svFitEta_ESCALEUP, "eta_sv_ESCALEUP/F");
      TBranch *newBranchEta1UU = t->Branch("eta_sv_UESUp", &svFitEta_UESUp, "eta_sv_UESUp/F");
      TBranch *newBranchEta1UD = t->Branch("eta_sv_UESDown", &svFitEta_UESDown, "eta_sv_UESDown/F");
      TBranch *newBranchEta1JetAbsoluteU = t->Branch("eta_sv_JetAbsoluteUp", &svFitEta_JetAbsoluteUp, "eta_sv_JetAbsoluteUp/F");
      TBranch *newBranchEta1JetAbsoluteD = t->Branch("eta_sv_JetAbsoluteDown", &svFitEta_JetAbsoluteDown, "eta_sv_JetAbsoluteDown/F");
      TBranch *newBranchEta1JetAbsoluteyearU = t->Branch("eta_sv_JetAbsoluteyearUp", &svFitEta_JetAbsoluteyearUp, "eta_sv_JetAbsoluteyearUp/F");
      TBranch *newBranchEta1JetAbsoluteyearD = t->Branch("eta_sv_JetAbsoluteyearDown", &svFitEta_JetAbsoluteyearDown, "eta_sv_JetAbsoluteyearDown/F");
      TBranch *newBranchEta1JetBBEC1U = t->Branch("eta_sv_JetBBEC1Up", &svFitEta_JetBBEC1Up, "eta_sv_JetBBEC1Up/F");
      TBranch *newBranchEta1JetBBEC1D = t->Branch("eta_sv_JetBBEC1Down", &svFitEta_JetBBEC1Down, "eta_sv_JetBBEC1Down/F");
      TBranch *newBranchEta1JetBBEC1yearU = t->Branch("eta_sv_JetBBEC1yearUp", &svFitEta_JetBBEC1yearUp, "eta_sv_JetBBEC1yearUp/F");
      TBranch *newBranchEta1JetBBEC1yearD = t->Branch("eta_sv_JetBBEC1yearDown", &svFitEta_JetBBEC1yearDown, "eta_sv_JetBBEC1yearDown/F");
      TBranch *newBranchEta1JetEC2U = t->Branch("eta_sv_JetEC2Up", &svFitEta_JetEC2Up, "eta_sv_JetEC2Up/F");
      TBranch *newBranchEta1JetEC2D = t->Branch("eta_sv_JetEC2Down", &svFitEta_JetEC2Down, "eta_sv_JetEC2Down/F");
      TBranch *newBranchEta1JetEC2yearU = t->Branch("eta_sv_JetEC2yearUp", &svFitEta_JetEC2yearUp, "eta_sv_JetEC2yearUp/F");
      TBranch *newBranchEta1JetEC2yearD = t->Branch("eta_sv_JetEC2yearDown", &svFitEta_JetEC2yearDown, "eta_sv_JetEC2yearDown/F");
      TBranch *newBranchEta1JetFlavorQCDU = t->Branch("eta_sv_JetFlavorQCDUp", &svFitEta_JetFlavorQCDUp, "eta_sv_JetFlavorQCDUp/F");
      TBranch *newBranchEta1JetFlavorQCDD = t->Branch("eta_sv_JetFlavorQCDDown", &svFitEta_JetFlavorQCDDown, "eta_sv_JetFlavorQCDDown/F");
      TBranch *newBranchEta1JetHFU = t->Branch("eta_sv_JetHFUp", &svFitEta_JetHFUp, "eta_sv_JetHFUp/F");
      TBranch *newBranchEta1JetHFD = t->Branch("eta_sv_JetHFDown", &svFitEta_JetHFDown, "eta_sv_JetHFDown/F");
      TBranch *newBranchEta1JetHFyearU = t->Branch("eta_sv_JetHFyearUp", &svFitEta_JetHFyearUp, "eta_sv_JetHFyearUp/F");
      TBranch *newBranchEta1JetHFyearD = t->Branch("eta_sv_JetHFyearDown", &svFitEta_JetHFyearDown, "eta_sv_JetHFyearDown/F");
      TBranch *newBranchEta1JetRelativeBalU = t->Branch("eta_sv_JetRelativeBalUp", &svFitEta_JetRelativeBalUp, "eta_sv_JetRelativeBalUp/F");
      TBranch *newBranchEta1JetRelativeBalD = t->Branch("eta_sv_JetRelativeBalDown", &svFitEta_JetRelativeBalDown, "eta_sv_JetRelativeBalDown/F");
      TBranch *newBranchEta1JetRelativeSampleU = t->Branch("eta_sv_JetRelativeSampleUp", &svFitEta_JetRelativeSampleUp, "eta_sv_JetRelativeSampleUp/F");
      TBranch *newBranchEta1JetRelativeSampleD = t->Branch("eta_sv_JetRelativeSampleDown", &svFitEta_JetRelativeSampleDown, "eta_sv_JetRelativeSampleDown/F");
      TBranch *newBranchEta1JERU = t->Branch("eta_sv_JERUp", &svFitEta_JERUp, "eta_sv_JERUp/F");
      TBranch *newBranchEta1JERD = t->Branch("eta_sv_JERDown", &svFitEta_JERDown, "eta_sv_JERDown/F");
      TBranch *newBranchEta1ResponseU = t->Branch("eta_sv_ResponseUp", &svFitEta_ResponseUp, "eta_sv_ResponseUp/F");
      TBranch *newBranchEta1ResponseD = t->Branch("eta_sv_ResponseDown", &svFitEta_ResponseDown, "eta_sv_ResponseDown/F");
      TBranch *newBranchEta1ResolutionU = t->Branch("eta_sv_ResolutionUp", &svFitEta_ResolutionUp, "eta_sv_ResolutionUp/F");
      TBranch *newBranchEta1ResolutionD = t->Branch("eta_sv_ResolutionDown", &svFitEta_ResolutionDown, "eta_sv_ResolutionDown/F");

      TBranch *newBranchPhi1 = t->Branch("phi_sv", &svFitPhi, "phi_sv/F");
      TBranch *newBranchPhi1D = t->Branch("phi_sv_DOWN", &svFitPhi_DOWN, "phi_sv_DOWN/F");
      TBranch *newBranchPhi1U = t->Branch("phi_sv_UP", &svFitPhi_UP, "phi_sv_UP/F");
      TBranch *newBranchPhi1ESCD = t->Branch("phi_sv_ESCALEDOWN", &svFitPhi_ESCALEDOWN, "phi_sv_ESCALEDOWN/F");
      TBranch *newBranchPhi1ESCU = t->Branch("phi_sv_ESCALEUP", &svFitPhi_ESCALEUP, "phi_sv_ESCALEUP/F");
      TBranch *newBranchPhi1UU = t->Branch("phi_sv_UESUp", &svFitPhi_UESUp, "phi_sv_UESUp/F");
      TBranch *newBranchPhi1UD = t->Branch("phi_sv_UESDown", &svFitPhi_UESDown, "phi_sv_UESDown/F");
      TBranch *newBranchPhi1JetAbsoluteU = t->Branch("phi_sv_JetAbsoluteUp", &svFitPhi_JetAbsoluteUp, "phi_sv_JetAbsoluteUp/F");
      TBranch *newBranchPhi1JetAbsoluteD = t->Branch("phi_sv_JetAbsoluteDown", &svFitPhi_JetAbsoluteDown, "phi_sv_JetAbsoluteDown/F");
      TBranch *newBranchPhi1JetAbsoluteyearU = t->Branch("phi_sv_JetAbsoluteyearUp", &svFitPhi_JetAbsoluteyearUp, "phi_sv_JetAbsoluteyearUp/F");
      TBranch *newBranchPhi1JetAbsoluteyearD = t->Branch("phi_sv_JetAbsoluteyearDown", &svFitPhi_JetAbsoluteyearDown, "phi_sv_JetAbsoluteyearDown/F");
      TBranch *newBranchPhi1JetBBEC1U = t->Branch("phi_sv_JetBBEC1Up", &svFitPhi_JetBBEC1Up, "phi_sv_JetBBEC1Up/F");
      TBranch *newBranchPhi1JetBBEC1D = t->Branch("phi_sv_JetBBEC1Down", &svFitPhi_JetBBEC1Down, "phi_sv_JetBBEC1Down/F");
      TBranch *newBranchPhi1JetBBEC1yearU = t->Branch("phi_sv_JetBBEC1yearUp", &svFitPhi_JetBBEC1yearUp, "phi_sv_JetBBEC1yearUp/F");
      TBranch *newBranchPhi1JetBBEC1yearD = t->Branch("phi_sv_JetBBEC1yearDown", &svFitPhi_JetBBEC1yearDown, "phi_sv_JetBBEC1yearDown/F");
      TBranch *newBranchPhi1JetEC2U = t->Branch("phi_sv_JetEC2Up", &svFitPhi_JetEC2Up, "phi_sv_JetEC2Up/F");
      TBranch *newBranchPhi1JetEC2D = t->Branch("phi_sv_JetEC2Down", &svFitPhi_JetEC2Down, "phi_sv_JetEC2Down/F");
      TBranch *newBranchPhi1JetEC2yearU = t->Branch("phi_sv_JetEC2yearUp", &svFitPhi_JetEC2yearUp, "phi_sv_JetEC2yearUp/F");
      TBranch *newBranchPhi1JetEC2yearD = t->Branch("phi_sv_JetEC2yearDown", &svFitPhi_JetEC2yearDown, "phi_sv_JetEC2yearDown/F");
      TBranch *newBranchPhi1JetFlavorQCDU = t->Branch("phi_sv_JetFlavorQCDUp", &svFitPhi_JetFlavorQCDUp, "phi_sv_JetFlavorQCDUp/F");
      TBranch *newBranchPhi1JetFlavorQCDD = t->Branch("phi_sv_JetFlavorQCDDown", &svFitPhi_JetFlavorQCDDown, "phi_sv_JetFlavorQCDDown/F");
      TBranch *newBranchPhi1JetHFU = t->Branch("phi_sv_JetHFUp", &svFitPhi_JetHFUp, "phi_sv_JetHFUp/F");
      TBranch *newBranchPhi1JetHFD = t->Branch("phi_sv_JetHFDown", &svFitPhi_JetHFDown, "phi_sv_JetHFDown/F");
      TBranch *newBranchPhi1JetHFyearU = t->Branch("phi_sv_JetHFyearUp", &svFitPhi_JetHFyearUp, "phi_sv_JetHFyearUp/F");
      TBranch *newBranchPhi1JetHFyearD = t->Branch("phi_sv_JetHFyearDown", &svFitPhi_JetHFyearDown, "phi_sv_JetHFyearDown/F");
      TBranch *newBranchPhi1JetRelativeBalU = t->Branch("phi_sv_JetRelativeBalUp", &svFitPhi_JetRelativeBalUp, "phi_sv_JetRelativeBalUp/F");
      TBranch *newBranchPhi1JetRelativeBalD = t->Branch("phi_sv_JetRelativeBalDown", &svFitPhi_JetRelativeBalDown, "phi_sv_JetRelativeBalDown/F");
      TBranch *newBranchPhi1JetRelativeSampleU = t->Branch("phi_sv_JetRelativeSampleUp", &svFitPhi_JetRelativeSampleUp, "phi_sv_JetRelativeSampleUp/F");
      TBranch *newBranchPhi1JetRelativeSampleD = t->Branch("phi_sv_JetRelativeSampleDown", &svFitPhi_JetRelativeSampleDown, "phi_sv_JetRelativeSampleDown/F");
      TBranch *newBranchPhi1JERU = t->Branch("phi_sv_JERUp", &svFitPhi_JERUp, "phi_sv_JERUp/F");
      TBranch *newBranchPhi1JERD = t->Branch("phi_sv_JERDown", &svFitPhi_JERDown, "phi_sv_JERDown/F");
      TBranch *newBranchPhi1ResponseU = t->Branch("phi_sv_ResponseUp", &svFitPhi_ResponseUp, "phi_sv_ResponseUp/F");
      TBranch *newBranchPhi1ResponseD = t->Branch("phi_sv_ResponseDown", &svFitPhi_ResponseDown, "phi_sv_ResponseDown/F");
      TBranch *newBranchPhi1ResolutionU = t->Branch("phi_sv_ResolutionUp", &svFitPhi_ResolutionUp, "phi_sv_ResolutionUp/F");
      TBranch *newBranchPhi1ResolutionD = t->Branch("phi_sv_ResolutionDown", &svFitPhi_ResolutionDown, "phi_sv_ResolutionDown/F");


      int evt;
      int run, lumi;
      float pt1;
      float pt1_scaleU;
      float pt1_scaleD;
      float eta1;
      float phi1;
      float pt2;
      float eta2;
      float phi2;
      float m2;
      int gen_match_2;
      float decayMode2;
      float pfCovMatrix00;
      float pfCovMatrix10;
      float pfCovMatrix01;
      float pfCovMatrix11;

      // define MET
      double measuredMETx = 0.;
      double measuredMETy = 0.;
      double measuredMETx_UESDown = 0.;
      double measuredMETy_UESDown = 0.;
      double measuredMETx_UESUp = 0.;
      double measuredMETy_UESUp = 0.;

      double measuredMETx_JetAbsoluteDown = 0.;
      double measuredMETy_JetAbsoluteDown = 0.;
      double measuredMETx_JetAbsoluteUp = 0.;
      double measuredMETy_JetAbsoluteUp = 0.;
      double measuredMETx_JetAbsoluteyearDown = 0.;
      double measuredMETy_JetAbsoluteyearDown = 0.;
      double measuredMETx_JetAbsoluteyearUp = 0.;
      double measuredMETy_JetAbsoluteyearUp = 0.;
      double measuredMETx_JetBBEC1Down = 0.;
      double measuredMETy_JetBBEC1Down = 0.;
      double measuredMETx_JetBBEC1Up = 0.;
      double measuredMETy_JetBBEC1Up = 0.;
      double measuredMETx_JetBBEC1yearDown = 0.;
      double measuredMETy_JetBBEC1yearDown = 0.;
      double measuredMETx_JetBBEC1yearUp = 0.;
      double measuredMETy_JetBBEC1yearUp = 0.;
      double measuredMETx_JetEC2Down = 0.;
      double measuredMETy_JetEC2Down = 0.;
      double measuredMETx_JetEC2Up = 0.;
      double measuredMETy_JetEC2Up = 0.;
      double measuredMETx_JetEC2yearDown = 0.;
      double measuredMETy_JetEC2yearDown = 0.;
      double measuredMETx_JetEC2yearUp = 0.;
      double measuredMETy_JetEC2yearUp = 0.;
      double measuredMETx_JetFlavorQCDDown = 0.;
      double measuredMETy_JetFlavorQCDDown = 0.;
      double measuredMETx_JetFlavorQCDUp = 0.;
      double measuredMETy_JetFlavorQCDUp = 0.;
      double measuredMETx_JetHFDown = 0.;
      double measuredMETy_JetHFDown = 0.;
      double measuredMETx_JetHFUp = 0.;
      double measuredMETy_JetHFUp = 0.;
      double measuredMETx_JetHFyearDown = 0.;
      double measuredMETy_JetHFyearDown = 0.;
      double measuredMETx_JetHFyearUp = 0.;
      double measuredMETy_JetHFyearUp = 0.;
      double measuredMETx_JetRelativeBalDown = 0.;
      double measuredMETy_JetRelativeBalDown = 0.;
      double measuredMETx_JetRelativeBalUp = 0.;
      double measuredMETy_JetRelativeBalUp = 0.;
      double measuredMETx_JetRelativeSampleDown = 0.;
      double measuredMETy_JetRelativeSampleDown = 0.;
      double measuredMETx_JetRelativeSampleUp = 0.;
      double measuredMETy_JetRelativeSampleUp = 0.;
      double measuredMETx_JERDown = 0.;
      double measuredMETy_JERDown = 0.;
      double measuredMETx_JERUp = 0.;
      double measuredMETy_JERUp = 0.;

      double measuredMETx_ResolutionDown = 0.;
      double measuredMETy_ResolutionDown = 0.;
      double measuredMETx_ResolutionUp = 0.;
      double measuredMETy_ResolutionUp = 0.;
      double measuredMETx_ResponseDown = 0.;
      double measuredMETy_ResponseDown = 0.;
      double measuredMETx_ResponseUp = 0.;
      double measuredMETy_ResponseUp = 0.;

      float pfmet;
      float pfmetphi;
      float pfmet_UESDown;
      float pfmetphi_UESDown;
      float pfmet_UESUp;
      float pfmetphi_UESUp;

      float pfmet_JetAbsoluteUp;
      float pfmetphi_JetAbsoluteUp;
      float pfmet_JetAbsoluteDown;
      float pfmetphi_JetAbsoluteDown;
      float pfmet_JetAbsoluteyearUp;
      float pfmetphi_JetAbsoluteyearUp;
      float pfmet_JetAbsoluteyearDown;
      float pfmetphi_JetAbsoluteyearDown;
      float pfmet_JetBBEC1Up;
      float pfmetphi_JetBBEC1Up;
      float pfmet_JetBBEC1Down;
      float pfmetphi_JetBBEC1Down;
      float pfmet_JetBBEC1yearUp;
      float pfmetphi_JetBBEC1yearUp;
      float pfmet_JetBBEC1yearDown;
      float pfmetphi_JetBBEC1yearDown;
      float pfmet_JetEC2Up;
      float pfmetphi_JetEC2Up;
      float pfmet_JetEC2Down;
      float pfmetphi_JetEC2Down;
      float pfmet_JetEC2yearUp;
      float pfmetphi_JetEC2yearUp;
      float pfmet_JetEC2yearDown;
      float pfmetphi_JetEC2yearDown;
      float pfmet_JetFlavorQCDUp;
      float pfmetphi_JetFlavorQCDUp;
      float pfmet_JetFlavorQCDDown;
      float pfmetphi_JetFlavorQCDDown;
      float pfmet_JetHFUp;
      float pfmetphi_JetHFUp;
      float pfmet_JetHFDown;
      float pfmetphi_JetHFDown;
      float pfmet_JetHFyearUp;
      float pfmetphi_JetHFyearUp;
      float pfmet_JetHFyearDown;
      float pfmetphi_JetHFyearDown;
      float pfmet_JetRelativeBalUp;
      float pfmetphi_JetRelativeBalUp;
      float pfmet_JetRelativeBalDown;
      float pfmetphi_JetRelativeBalDown;
      float pfmet_JetRelativeSampleUp;
      float pfmetphi_JetRelativeSampleUp;
      float pfmet_JetRelativeSampleDown;
      float pfmetphi_JetRelativeSampleDown;
      float pfmet_JERUp;
      float pfmetphi_JERUp;
      float pfmet_JERDown;
      float pfmetphi_JERDown;

      float pfmet_ResponseDown;
      float pfmetphi_ResponseDown;
      float pfmet_ResponseUp;
      float pfmetphi_ResponseUp;
      float pfmet_ResolutionDown;
      float pfmetphi_ResolutionDown;
      float pfmet_ResolutionUp;
      float pfmetphi_ResolutionUp;

      TLorentzVector TMet(0,0,0,0);
      TLorentzVector TMet_UESDown(0,0,0,0);
      TLorentzVector TMet_UESUp(0,0,0,0);

      TLorentzVector TMet_JetAbsoluteUp(0,0,0,0);
      TLorentzVector TMet_JetAbsoluteDown(0,0,0,0);
      TLorentzVector TMet_JetAbsoluteyearUp(0,0,0,0);
      TLorentzVector TMet_JetAbsoluteyearDown(0,0,0,0);
      TLorentzVector TMet_JetBBEC1Up(0,0,0,0);
      TLorentzVector TMet_JetBBEC1Down(0,0,0,0);
      TLorentzVector TMet_JetBBEC1yearUp(0,0,0,0);
      TLorentzVector TMet_JetBBEC1yearDown(0,0,0,0);
      TLorentzVector TMet_JetEC2Up(0,0,0,0);
      TLorentzVector TMet_JetEC2Down(0,0,0,0);
      TLorentzVector TMet_JetEC2yearUp(0,0,0,0);
      TLorentzVector TMet_JetEC2yearDown(0,0,0,0);
      TLorentzVector TMet_JetFlavorQCDUp(0,0,0,0);
      TLorentzVector TMet_JetFlavorQCDDown(0,0,0,0);
      TLorentzVector TMet_JetHFUp(0,0,0,0);
      TLorentzVector TMet_JetHFDown(0,0,0,0);
      TLorentzVector TMet_JetHFyearUp(0,0,0,0);
      TLorentzVector TMet_JetHFyearDown(0,0,0,0);
      TLorentzVector TMet_JetRelativeBalUp(0,0,0,0);
      TLorentzVector TMet_JetRelativeBalDown(0,0,0,0);
      TLorentzVector TMet_JetRelativeSampleUp(0,0,0,0);
      TLorentzVector TMet_JetRelativeSampleDown(0,0,0,0);
      TLorentzVector TMet_JERUp(0,0,0,0);
      TLorentzVector TMet_JERDown(0,0,0,0);

      TLorentzVector TMet_ResolutionDown(0,0,0,0);
      TLorentzVector TMet_ResolutionUp(0,0,0,0);
      TLorentzVector TMet_ResponseDown(0,0,0,0);
      TLorentzVector TMet_ResponseUp(0,0,0,0);

      TMatrixD covMET(2, 2);

      t->SetBranchAddress("evt",&evt);
      t->SetBranchAddress("run",&run);
      t->SetBranchAddress("lumi",&lumi);
      t->SetBranchAddress("pt_1",&pt1);
      t->SetBranchAddress("pt_1_ScaleUp",&pt1_scaleU);
      t->SetBranchAddress("pt_1_ScaleDown",&pt1_scaleD);
      t->SetBranchAddress("eta_1",&eta1);
      t->SetBranchAddress("phi_1",&phi1);
      t->SetBranchAddress("pt_2",&pt2);
      t->SetBranchAddress("eta_2",&eta2);
      t->SetBranchAddress("phi_2",&phi2);
      t->SetBranchAddress("m_2",&m2);
      t->SetBranchAddress("gen_match_2",&gen_match_2);
      t->SetBranchAddress("l2_decayMode",&decayMode2);
      t->SetBranchAddress("met",&pfmet);
      t->SetBranchAddress("metphi",&pfmetphi);
      t->SetBranchAddress("met_UESUp",&pfmet_UESUp);
      t->SetBranchAddress("metphi_UESUp",&pfmetphi_UESUp);
      t->SetBranchAddress("met_UESDown",&pfmet_UESDown);
      t->SetBranchAddress("metphi_UESDown",&pfmetphi_UESDown);
      t->SetBranchAddress("met_responseUp",&pfmet_ResponseUp);
      t->SetBranchAddress("metphi_responseUp",&pfmetphi_ResponseUp);
      t->SetBranchAddress("met_responseDown",&pfmet_ResponseDown);
      t->SetBranchAddress("metphi_responseDown",&pfmetphi_ResponseDown);
      t->SetBranchAddress("met_resolutionUp",&pfmet_ResolutionUp);
      t->SetBranchAddress("metphi_resolutionUp",&pfmetphi_ResolutionUp);
      t->SetBranchAddress("met_resolutionDown",&pfmet_ResolutionDown);
      t->SetBranchAddress("metphi_resolutionDown",&pfmetphi_ResolutionDown);

      t->SetBranchAddress("met_JetAbsoluteUp",&pfmet_JetAbsoluteUp);
      t->SetBranchAddress("met_JetAbsoluteDown",&pfmet_JetAbsoluteDown);
      t->SetBranchAddress("metphi_JetAbsoluteUp",&pfmetphi_JetAbsoluteUp);
      t->SetBranchAddress("metphi_JetAbsoluteDown",&pfmetphi_JetAbsoluteDown);
      t->SetBranchAddress("met_JetAbsoluteyearUp",&pfmet_JetAbsoluteyearUp);
      t->SetBranchAddress("met_JetAbsoluteyearDown",&pfmet_JetAbsoluteyearDown);
      t->SetBranchAddress("metphi_JetAbsoluteyearUp",&pfmetphi_JetAbsoluteyearUp);
      t->SetBranchAddress("metphi_JetAbsoluteyearDown",&pfmetphi_JetAbsoluteyearDown);
      t->SetBranchAddress("met_JetBBEC1Up",&pfmet_JetBBEC1Up);
      t->SetBranchAddress("met_JetBBEC1Down",&pfmet_JetBBEC1Down);
      t->SetBranchAddress("metphi_JetBBEC1Up",&pfmetphi_JetBBEC1Up);
      t->SetBranchAddress("metphi_JetBBEC1Down",&pfmetphi_JetBBEC1Down);
      t->SetBranchAddress("met_JetBBEC1yearUp",&pfmet_JetBBEC1yearUp);
      t->SetBranchAddress("met_JetBBEC1yearDown",&pfmet_JetBBEC1yearDown);
      t->SetBranchAddress("metphi_JetBBEC1yearUp",&pfmetphi_JetBBEC1yearUp);
      t->SetBranchAddress("metphi_JetBBEC1yearDown",&pfmetphi_JetBBEC1yearDown);
      t->SetBranchAddress("met_JetEC2Up",&pfmet_JetEC2Up);
      t->SetBranchAddress("met_JetEC2Down",&pfmet_JetEC2Down);
      t->SetBranchAddress("metphi_JetEC2Up",&pfmetphi_JetEC2Up);
      t->SetBranchAddress("metphi_JetEC2Down",&pfmetphi_JetEC2Down);
      t->SetBranchAddress("met_JetEC2yearUp",&pfmet_JetEC2yearUp);
      t->SetBranchAddress("met_JetEC2yearDown",&pfmet_JetEC2yearDown);
      t->SetBranchAddress("metphi_JetEC2yearUp",&pfmetphi_JetEC2yearUp);
      t->SetBranchAddress("metphi_JetEC2yearDown",&pfmetphi_JetEC2yearDown);
      t->SetBranchAddress("met_JetFlavorQCDUp",&pfmet_JetFlavorQCDUp);
      t->SetBranchAddress("met_JetFlavorQCDDown",&pfmet_JetFlavorQCDDown);
      t->SetBranchAddress("metphi_JetFlavorQCDUp",&pfmetphi_JetFlavorQCDUp);
      t->SetBranchAddress("metphi_JetFlavorQCDDown",&pfmetphi_JetFlavorQCDDown);
      t->SetBranchAddress("met_JetHFUp",&pfmet_JetHFUp);
      t->SetBranchAddress("met_JetHFDown",&pfmet_JetHFDown);
      t->SetBranchAddress("metphi_JetHFUp",&pfmetphi_JetHFUp);
      t->SetBranchAddress("metphi_JetHFDown",&pfmetphi_JetHFDown);
      t->SetBranchAddress("met_JetHFyearUp",&pfmet_JetHFyearUp);
      t->SetBranchAddress("met_JetHFyearDown",&pfmet_JetHFyearDown);
      t->SetBranchAddress("metphi_JetHFyearUp",&pfmetphi_JetHFyearUp);
      t->SetBranchAddress("metphi_JetHFyearDown",&pfmetphi_JetHFyearDown);
      t->SetBranchAddress("met_JetRelativeBalUp",&pfmet_JetRelativeBalUp);
      t->SetBranchAddress("met_JetRelativeBalDown",&pfmet_JetRelativeBalDown);
      t->SetBranchAddress("metphi_JetRelativeBalUp",&pfmetphi_JetRelativeBalUp);
      t->SetBranchAddress("metphi_JetRelativeBalDown",&pfmetphi_JetRelativeBalDown);
      t->SetBranchAddress("met_JetRelativeSampleUp",&pfmet_JetRelativeSampleUp);
      t->SetBranchAddress("met_JetRelativeSampleDown",&pfmet_JetRelativeSampleDown);
      t->SetBranchAddress("metphi_JetRelativeSampleUp",&pfmetphi_JetRelativeSampleUp);
      t->SetBranchAddress("metphi_JetRelativeSampleDown",&pfmetphi_JetRelativeSampleDown);
      t->SetBranchAddress("met_JERUp",&pfmet_JERUp);
      t->SetBranchAddress("met_JERDown",&pfmet_JERDown);
      t->SetBranchAddress("metphi_JERUp",&pfmetphi_JERUp);
      t->SetBranchAddress("metphi_JERDown",&pfmetphi_JERDown);

      // FOR PF MET ANALYSIS
      t->SetBranchAddress("metcov00",&pfCovMatrix00);
      t->SetBranchAddress("metcov01",&pfCovMatrix01);
      t->SetBranchAddress("metcov10",&pfCovMatrix10);
      t->SetBranchAddress("metcov11",&pfCovMatrix11);

      printf("Found tree -> weighting\n");

      /*int verbosity = 1;
      ClassicSVfit svFitAlgo(verbosity);
      svFitAlgo.addLogM_fixed(true, 6.);
      svFitAlgo.setLikelihoodFileName("testClassicSVfit.root");*/

      for(Int_t i=0;i<t->GetEntries();++i){
         t->GetEntry(i);

         TMet.SetPtEtaPhiM(pfmet,0,pfmetphi,0);
         measuredMETx = pfmet*TMath::Cos(pfmetphi);
         measuredMETy = pfmet*TMath::Sin(pfmetphi);

         TMet_UESUp.SetPtEtaPhiM(pfmet_UESUp,0,pfmetphi_UESUp,0);
         measuredMETx_UESUp = pfmet_UESUp*TMath::Cos(pfmetphi_UESUp);
         measuredMETy_UESUp = pfmet_UESUp*TMath::Sin(pfmetphi_UESUp);
         TMet_UESDown.SetPtEtaPhiM(pfmet_UESDown,0,pfmetphi_UESDown,0);
         measuredMETx_UESDown = pfmet_UESDown*TMath::Cos(pfmetphi_UESDown);
         measuredMETy_UESDown = pfmet_UESDown*TMath::Sin(pfmetphi_UESDown);

         TMet_ResolutionUp.SetPtEtaPhiM(pfmet_ResolutionUp,0,pfmetphi_ResolutionUp,0);
         measuredMETx_ResolutionUp = pfmet_ResolutionUp*TMath::Cos(pfmetphi_ResolutionUp);
         measuredMETy_ResolutionUp = pfmet_ResolutionUp*TMath::Sin(pfmetphi_ResolutionUp);
         TMet_ResolutionDown.SetPtEtaPhiM(pfmet_ResolutionDown,0,pfmetphi_ResolutionDown,0);
         measuredMETx_ResolutionDown = pfmet_ResolutionDown*TMath::Cos(pfmetphi_ResolutionDown);
         measuredMETy_ResolutionDown = pfmet_ResolutionDown*TMath::Sin(pfmetphi_ResolutionDown);
         TMet_ResponseUp.SetPtEtaPhiM(pfmet_ResponseUp,0,pfmetphi_ResponseUp,0);
         measuredMETx_ResponseUp = pfmet_ResponseUp*TMath::Cos(pfmetphi_ResponseUp);
         measuredMETy_ResponseUp = pfmet_ResponseUp*TMath::Sin(pfmetphi_ResponseUp);
         TMet_ResponseDown.SetPtEtaPhiM(pfmet_ResponseDown,0,pfmetphi_ResponseDown,0);
         measuredMETx_ResponseDown = pfmet_ResponseDown*TMath::Cos(pfmetphi_ResponseDown);
         measuredMETy_ResponseDown = pfmet_ResponseDown*TMath::Sin(pfmetphi_ResponseDown);

         TMet_JetAbsoluteUp.SetPtEtaPhiM(pfmet_JetAbsoluteUp,0,pfmetphi_JetAbsoluteUp,0);
         measuredMETx_JetAbsoluteUp = pfmet_JetAbsoluteUp*TMath::Cos(pfmetphi_JetAbsoluteUp);
         measuredMETy_JetAbsoluteUp = pfmet_JetAbsoluteUp*TMath::Sin(pfmetphi_JetAbsoluteUp);
         TMet_JetAbsoluteDown.SetPtEtaPhiM(pfmet_JetAbsoluteDown,0,pfmetphi_JetAbsoluteDown,0);
         measuredMETx_JetAbsoluteDown = pfmet_JetAbsoluteDown*TMath::Cos(pfmetphi_JetAbsoluteDown);
         measuredMETy_JetAbsoluteDown = pfmet_JetAbsoluteDown*TMath::Sin(pfmetphi_JetAbsoluteDown);
         TMet_JetAbsoluteyearUp.SetPtEtaPhiM(pfmet_JetAbsoluteyearUp,0,pfmetphi_JetAbsoluteyearUp,0);
         measuredMETx_JetAbsoluteyearUp = pfmet_JetAbsoluteyearUp*TMath::Cos(pfmetphi_JetAbsoluteyearUp);
         measuredMETy_JetAbsoluteyearUp = pfmet_JetAbsoluteyearUp*TMath::Sin(pfmetphi_JetAbsoluteyearUp);
         TMet_JetAbsoluteyearDown.SetPtEtaPhiM(pfmet_JetAbsoluteyearDown,0,pfmetphi_JetAbsoluteyearDown,0);
         measuredMETx_JetAbsoluteyearDown = pfmet_JetAbsoluteyearDown*TMath::Cos(pfmetphi_JetAbsoluteyearDown);
         measuredMETy_JetAbsoluteyearDown = pfmet_JetAbsoluteyearDown*TMath::Sin(pfmetphi_JetAbsoluteyearDown);
         TMet_JetBBEC1Up.SetPtEtaPhiM(pfmet_JetBBEC1Up,0,pfmetphi_JetBBEC1Up,0);
         measuredMETx_JetBBEC1Up = pfmet_JetBBEC1Up*TMath::Cos(pfmetphi_JetBBEC1Up);
         measuredMETy_JetBBEC1Up = pfmet_JetBBEC1Up*TMath::Sin(pfmetphi_JetBBEC1Up);
         TMet_JetBBEC1Down.SetPtEtaPhiM(pfmet_JetBBEC1Down,0,pfmetphi_JetBBEC1Down,0);
         measuredMETx_JetBBEC1Down = pfmet_JetBBEC1Down*TMath::Cos(pfmetphi_JetBBEC1Down);
         measuredMETy_JetBBEC1Down = pfmet_JetBBEC1Down*TMath::Sin(pfmetphi_JetBBEC1Down);
         TMet_JetBBEC1yearUp.SetPtEtaPhiM(pfmet_JetBBEC1yearUp,0,pfmetphi_JetBBEC1yearUp,0);
         measuredMETx_JetBBEC1yearUp = pfmet_JetBBEC1yearUp*TMath::Cos(pfmetphi_JetBBEC1yearUp);
         measuredMETy_JetBBEC1yearUp = pfmet_JetBBEC1yearUp*TMath::Sin(pfmetphi_JetBBEC1yearUp);
         TMet_JetBBEC1yearDown.SetPtEtaPhiM(pfmet_JetBBEC1yearDown,0,pfmetphi_JetBBEC1yearDown,0);
         measuredMETx_JetBBEC1yearDown = pfmet_JetBBEC1yearDown*TMath::Cos(pfmetphi_JetBBEC1yearDown);
         measuredMETy_JetBBEC1yearDown = pfmet_JetBBEC1yearDown*TMath::Sin(pfmetphi_JetBBEC1yearDown);
         TMet_JetEC2Up.SetPtEtaPhiM(pfmet_JetEC2Up,0,pfmetphi_JetEC2Up,0);
         measuredMETx_JetEC2Up = pfmet_JetEC2Up*TMath::Cos(pfmetphi_JetEC2Up);
         measuredMETy_JetEC2Up = pfmet_JetEC2Up*TMath::Sin(pfmetphi_JetEC2Up);
         TMet_JetEC2Down.SetPtEtaPhiM(pfmet_JetEC2Down,0,pfmetphi_JetEC2Down,0);
         measuredMETx_JetEC2Down = pfmet_JetEC2Down*TMath::Cos(pfmetphi_JetEC2Down);
         measuredMETy_JetEC2Down = pfmet_JetEC2Down*TMath::Sin(pfmetphi_JetEC2Down);
         TMet_JetEC2yearUp.SetPtEtaPhiM(pfmet_JetEC2yearUp,0,pfmetphi_JetEC2yearUp,0);
         measuredMETx_JetEC2yearUp = pfmet_JetEC2yearUp*TMath::Cos(pfmetphi_JetEC2yearUp);
         measuredMETy_JetEC2yearUp = pfmet_JetEC2yearUp*TMath::Sin(pfmetphi_JetEC2yearUp);
         TMet_JetEC2yearDown.SetPtEtaPhiM(pfmet_JetEC2yearDown,0,pfmetphi_JetEC2yearDown,0);
         measuredMETx_JetEC2yearDown = pfmet_JetEC2yearDown*TMath::Cos(pfmetphi_JetEC2yearDown);
         measuredMETy_JetEC2yearDown = pfmet_JetEC2yearDown*TMath::Sin(pfmetphi_JetEC2yearDown);
         TMet_JetFlavorQCDUp.SetPtEtaPhiM(pfmet_JetFlavorQCDUp,0,pfmetphi_JetFlavorQCDUp,0);
         measuredMETx_JetFlavorQCDUp = pfmet_JetFlavorQCDUp*TMath::Cos(pfmetphi_JetFlavorQCDUp);
         measuredMETy_JetFlavorQCDUp = pfmet_JetFlavorQCDUp*TMath::Sin(pfmetphi_JetFlavorQCDUp);
         TMet_JetFlavorQCDDown.SetPtEtaPhiM(pfmet_JetFlavorQCDDown,0,pfmetphi_JetFlavorQCDDown,0);
         measuredMETx_JetFlavorQCDDown = pfmet_JetFlavorQCDDown*TMath::Cos(pfmetphi_JetFlavorQCDDown);
         measuredMETy_JetFlavorQCDDown = pfmet_JetFlavorQCDDown*TMath::Sin(pfmetphi_JetFlavorQCDDown);
         TMet_JetHFUp.SetPtEtaPhiM(pfmet_JetHFUp,0,pfmetphi_JetHFUp,0);
         measuredMETx_JetHFUp = pfmet_JetHFUp*TMath::Cos(pfmetphi_JetHFUp);
         measuredMETy_JetHFUp = pfmet_JetHFUp*TMath::Sin(pfmetphi_JetHFUp);
         TMet_JetHFDown.SetPtEtaPhiM(pfmet_JetHFDown,0,pfmetphi_JetHFDown,0);
         measuredMETx_JetHFDown = pfmet_JetHFDown*TMath::Cos(pfmetphi_JetHFDown);
         measuredMETy_JetHFDown = pfmet_JetHFDown*TMath::Sin(pfmetphi_JetHFDown);
         TMet_JetHFyearUp.SetPtEtaPhiM(pfmet_JetHFyearUp,0,pfmetphi_JetHFyearUp,0);
         measuredMETx_JetHFyearUp = pfmet_JetHFyearUp*TMath::Cos(pfmetphi_JetHFyearUp);
         measuredMETy_JetHFyearUp = pfmet_JetHFyearUp*TMath::Sin(pfmetphi_JetHFyearUp);
         TMet_JetHFyearDown.SetPtEtaPhiM(pfmet_JetHFyearDown,0,pfmetphi_JetHFyearDown,0);
         measuredMETx_JetHFyearDown = pfmet_JetHFyearDown*TMath::Cos(pfmetphi_JetHFyearDown);
         measuredMETy_JetHFyearDown = pfmet_JetHFyearDown*TMath::Sin(pfmetphi_JetHFyearDown);
         TMet_JetRelativeBalUp.SetPtEtaPhiM(pfmet_JetRelativeBalUp,0,pfmetphi_JetRelativeBalUp,0);
         measuredMETx_JetRelativeBalUp = pfmet_JetRelativeBalUp*TMath::Cos(pfmetphi_JetRelativeBalUp);
         measuredMETy_JetRelativeBalUp = pfmet_JetRelativeBalUp*TMath::Sin(pfmetphi_JetRelativeBalUp);
         TMet_JetRelativeBalDown.SetPtEtaPhiM(pfmet_JetRelativeBalDown,0,pfmetphi_JetRelativeBalDown,0);
         measuredMETx_JetRelativeBalDown = pfmet_JetRelativeBalDown*TMath::Cos(pfmetphi_JetRelativeBalDown);
         measuredMETy_JetRelativeBalDown = pfmet_JetRelativeBalDown*TMath::Sin(pfmetphi_JetRelativeBalDown);
         TMet_JetRelativeSampleUp.SetPtEtaPhiM(pfmet_JetRelativeSampleUp,0,pfmetphi_JetRelativeSampleUp,0);
         measuredMETx_JetRelativeSampleUp = pfmet_JetRelativeSampleUp*TMath::Cos(pfmetphi_JetRelativeSampleUp);
         measuredMETy_JetRelativeSampleUp = pfmet_JetRelativeSampleUp*TMath::Sin(pfmetphi_JetRelativeSampleUp);
         TMet_JetRelativeSampleDown.SetPtEtaPhiM(pfmet_JetRelativeSampleDown,0,pfmetphi_JetRelativeSampleDown,0);
         measuredMETx_JetRelativeSampleDown = pfmet_JetRelativeSampleDown*TMath::Cos(pfmetphi_JetRelativeSampleDown);
         measuredMETy_JetRelativeSampleDown = pfmet_JetRelativeSampleDown*TMath::Sin(pfmetphi_JetRelativeSampleDown);
         TMet_JERUp.SetPtEtaPhiM(pfmet_JERUp,0,pfmetphi_JERUp,0);
         measuredMETx_JERUp = pfmet_JERUp*TMath::Cos(pfmetphi_JERUp);
         measuredMETy_JERUp = pfmet_JERUp*TMath::Sin(pfmetphi_JERUp);
         TMet_JERDown.SetPtEtaPhiM(pfmet_JERDown,0,pfmetphi_JERDown,0);
         measuredMETx_JERDown = pfmet_JERDown*TMath::Cos(pfmetphi_JERDown);
         measuredMETy_JERDown = pfmet_JERDown*TMath::Sin(pfmetphi_JERDown);


         covMET[0][0] =  pfCovMatrix00;
         covMET[1][0] =  pfCovMatrix10;
         covMET[0][1] =  pfCovMatrix01;
         covMET[1][1] =  pfCovMatrix11;

	 covMET[0][0] =  300;//FIXME XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
         covMET[1][1] =  300;

	 std::vector<classic_svFit::MeasuredTauLepton> measuredTauLeptons;
	 measuredTauLeptons.push_back(classic_svFit::MeasuredTauLepton(classic_svFit::MeasuredTauLepton::kTauToElecDecay, pt1, eta1,  phi1, 0.51100e-3)); 
         measuredTauLeptons.push_back(classic_svFit::MeasuredTauLepton(classic_svFit::MeasuredTauLepton::kTauToHadDecay,  pt2, eta2, phi2, m2, decayMode2));
         runSVFit(measuredTauLeptons, measuredMETx, measuredMETy, covMET, 0, svFitMass, svFitPt, svFitEta, svFitPhi);

	 if (doUES){
	     runSVFit(measuredTauLeptons, measuredMETx_UESUp, measuredMETy_UESUp, covMET, 0, svFitMass_UESUp, svFitPt_UESUp, svFitEta_UESUp, svFitPhi_UESUp);
             runSVFit(measuredTauLeptons, measuredMETx_UESDown, measuredMETy_UESDown, covMET, 0, svFitMass_UESDown, svFitPt_UESDown, svFitEta_UESDown, svFitPhi_UESDown);
	 }
	 else{
	    svFitMass_UESUp=svFitMass;
            svFitMass_UESDown=svFitMass;
            svFitPt_UESUp=svFitPt;
            svFitPt_UESDown=svFitPt;
            svFitEta_UESUp=svFitEta;
            svFitEta_UESDown=svFitEta;
            svFitPhi_UESUp=svFitPhi;
            svFitPhi_UESDown=svFitPhi;
	 }

         if (doRES){
             runSVFit(measuredTauLeptons, measuredMETx_ResolutionUp, measuredMETy_ResolutionUp, covMET, 0, svFitMass_ResolutionUp, svFitPt_ResolutionUp, svFitEta_ResolutionUp, svFitPhi_ResolutionUp);
             runSVFit(measuredTauLeptons, measuredMETx_ResolutionDown, measuredMETy_ResolutionDown, covMET, 0, svFitMass_ResolutionDown, svFitPt_ResolutionDown, svFitEta_ResolutionDown, svFitPhi_ResolutionDown);
             runSVFit(measuredTauLeptons, measuredMETx_ResponseUp, measuredMETy_ResponseUp, covMET, 0, svFitMass_ResponseUp, svFitPt_ResponseUp, svFitEta_ResponseUp, svFitPhi_ResponseUp);
             runSVFit(measuredTauLeptons, measuredMETx_ResponseDown, measuredMETy_ResponseDown, covMET, 0, svFitMass_ResponseDown, svFitPt_ResponseDown, svFitEta_ResponseDown, svFitPhi_ResponseDown);
         }
         else{
            svFitMass_ResponseUp=svFitMass;
            svFitMass_ResponseDown=svFitMass;
            svFitMass_ResolutionUp=svFitMass;
            svFitMass_ResolutionDown=svFitMass;
            svFitPt_ResponseUp=svFitPt;
            svFitPt_ResponseDown=svFitPt;
            svFitPt_ResolutionUp=svFitPt;
            svFitPt_ResolutionDown=svFitPt;
            svFitEta_ResponseUp=svFitEta;
            svFitEta_ResponseDown=svFitEta;
            svFitEta_ResolutionUp=svFitEta;
            svFitEta_ResolutionDown=svFitEta;
            svFitPhi_ResponseUp=svFitPhi;
            svFitPhi_ResponseDown=svFitPhi;
            svFitPhi_ResolutionUp=svFitPhi;
            svFitPhi_ResolutionDown=svFitPhi;
         }

         if (doJES){
             runSVFit(measuredTauLeptons, measuredMETx_JetAbsoluteUp, measuredMETy_JetAbsoluteUp, covMET, 0, svFitMass_JetAbsoluteUp, svFitPt_JetAbsoluteUp, svFitEta_JetAbsoluteUp, svFitPhi_JetAbsoluteUp);
             runSVFit(measuredTauLeptons, measuredMETx_JetAbsoluteDown, measuredMETy_JetAbsoluteDown, covMET, 0, svFitMass_JetAbsoluteDown, svFitPt_JetAbsoluteDown, svFitEta_JetAbsoluteDown, svFitPhi_JetAbsoluteDown);
             runSVFit(measuredTauLeptons, measuredMETx_JetAbsoluteyearUp, measuredMETy_JetAbsoluteyearUp, covMET, 0, svFitMass_JetAbsoluteyearUp, svFitPt_JetAbsoluteyearUp, svFitEta_JetAbsoluteyearUp, svFitPhi_JetAbsoluteyearUp);
             runSVFit(measuredTauLeptons, measuredMETx_JetAbsoluteyearDown, measuredMETy_JetAbsoluteyearDown, covMET, 0, svFitMass_JetAbsoluteyearDown, svFitPt_JetAbsoluteyearDown, svFitEta_JetAbsoluteyearDown, svFitPhi_JetAbsoluteyearDown);
             runSVFit(measuredTauLeptons, measuredMETx_JetBBEC1Up, measuredMETy_JetBBEC1Up, covMET, 0, svFitMass_JetBBEC1Up, svFitPt_JetBBEC1Up, svFitEta_JetBBEC1Up, svFitPhi_JetBBEC1Up);
             runSVFit(measuredTauLeptons, measuredMETx_JetBBEC1Down, measuredMETy_JetBBEC1Down, covMET, 0, svFitMass_JetBBEC1Down, svFitPt_JetBBEC1Down, svFitEta_JetBBEC1Down, svFitPhi_JetBBEC1Down);
             runSVFit(measuredTauLeptons, measuredMETx_JetBBEC1yearUp, measuredMETy_JetBBEC1yearUp, covMET, 0, svFitMass_JetBBEC1yearUp, svFitPt_JetBBEC1yearUp, svFitEta_JetBBEC1yearUp, svFitPhi_JetBBEC1yearUp);
             runSVFit(measuredTauLeptons, measuredMETx_JetBBEC1yearDown, measuredMETy_JetBBEC1yearDown, covMET, 0, svFitMass_JetBBEC1yearDown, svFitPt_JetBBEC1yearDown, svFitEta_JetBBEC1yearDown, svFitPhi_JetBBEC1yearDown);
             runSVFit(measuredTauLeptons, measuredMETx_JetEC2Up, measuredMETy_JetEC2Up, covMET, 0, svFitMass_JetEC2Up, svFitPt_JetEC2Up, svFitEta_JetEC2Up, svFitPhi_JetEC2Up);
             runSVFit(measuredTauLeptons, measuredMETx_JetEC2Down, measuredMETy_JetEC2Down, covMET, 0, svFitMass_JetEC2Down, svFitPt_JetEC2Down, svFitEta_JetEC2Down, svFitPhi_JetEC2Down);
             runSVFit(measuredTauLeptons, measuredMETx_JetEC2yearUp, measuredMETy_JetEC2yearUp, covMET, 0, svFitMass_JetEC2yearUp, svFitPt_JetEC2yearUp, svFitEta_JetEC2yearUp, svFitPhi_JetEC2yearUp);
             runSVFit(measuredTauLeptons, measuredMETx_JetEC2yearDown, measuredMETy_JetEC2yearDown, covMET, 0, svFitMass_JetEC2yearDown, svFitPt_JetEC2yearDown, svFitEta_JetEC2yearDown, svFitPhi_JetEC2yearDown);
             runSVFit(measuredTauLeptons, measuredMETx_JetFlavorQCDUp, measuredMETy_JetFlavorQCDUp, covMET, 0, svFitMass_JetFlavorQCDUp, svFitPt_JetFlavorQCDUp, svFitEta_JetFlavorQCDUp, svFitPhi_JetFlavorQCDUp);
             runSVFit(measuredTauLeptons, measuredMETx_JetFlavorQCDDown, measuredMETy_JetFlavorQCDDown, covMET, 0, svFitMass_JetFlavorQCDDown, svFitPt_JetFlavorQCDDown, svFitEta_JetFlavorQCDDown, svFitPhi_JetFlavorQCDDown);
             runSVFit(measuredTauLeptons, measuredMETx_JetHFUp, measuredMETy_JetHFUp, covMET, 0, svFitMass_JetHFUp, svFitPt_JetHFUp, svFitEta_JetHFUp, svFitPhi_JetHFUp);
             runSVFit(measuredTauLeptons, measuredMETx_JetHFDown, measuredMETy_JetHFDown, covMET, 0, svFitMass_JetHFDown, svFitPt_JetHFDown, svFitEta_JetHFDown, svFitPhi_JetHFDown);
             runSVFit(measuredTauLeptons, measuredMETx_JetHFyearUp, measuredMETy_JetHFyearUp, covMET, 0, svFitMass_JetHFyearUp, svFitPt_JetHFyearUp, svFitEta_JetHFyearUp, svFitPhi_JetHFyearUp);
             runSVFit(measuredTauLeptons, measuredMETx_JetHFyearDown, measuredMETy_JetHFyearDown, covMET, 0, svFitMass_JetHFyearDown, svFitPt_JetHFyearDown, svFitEta_JetHFyearDown, svFitPhi_JetHFyearDown);
             runSVFit(measuredTauLeptons, measuredMETx_JetRelativeBalUp, measuredMETy_JetRelativeBalUp, covMET, 0, svFitMass_JetRelativeBalUp, svFitPt_JetRelativeBalUp, svFitEta_JetRelativeBalUp, svFitPhi_JetRelativeBalUp);
             runSVFit(measuredTauLeptons, measuredMETx_JetRelativeBalDown, measuredMETy_JetRelativeBalDown, covMET, 0, svFitMass_JetRelativeBalDown, svFitPt_JetRelativeBalDown, svFitEta_JetRelativeBalDown, svFitPhi_JetRelativeBalDown);
             runSVFit(measuredTauLeptons, measuredMETx_JetRelativeSampleUp, measuredMETy_JetRelativeSampleUp, covMET, 0, svFitMass_JetRelativeSampleUp, svFitPt_JetRelativeSampleUp, svFitEta_JetRelativeSampleUp, svFitPhi_JetRelativeSampleUp);
             runSVFit(measuredTauLeptons, measuredMETx_JetRelativeSampleDown, measuredMETy_JetRelativeSampleDown, covMET, 0, svFitMass_JetRelativeSampleDown, svFitPt_JetRelativeSampleDown, svFitEta_JetRelativeSampleDown, svFitPhi_JetRelativeSampleDown);
             runSVFit(measuredTauLeptons, measuredMETx_JERUp, measuredMETy_JERUp, covMET, 0, svFitMass_JERUp, svFitPt_JERUp, svFitEta_JERUp, svFitPhi_JERUp);
             runSVFit(measuredTauLeptons, measuredMETx_JERDown, measuredMETy_JERDown, covMET, 0, svFitMass_JERDown, svFitPt_JERDown, svFitEta_JERDown, svFitPhi_JERDown);
         }
         else{
            svFitMass_JetAbsoluteUp=svFitMass;
            svFitMass_JetAbsoluteDown=svFitMass;
            svFitMass_JetAbsoluteyearUp=svFitMass;
            svFitMass_JetAbsoluteyearDown=svFitMass;
            svFitMass_JetBBEC1Up=svFitMass;
            svFitMass_JetBBEC1Down=svFitMass;
            svFitMass_JetBBEC1yearUp=svFitMass;
            svFitMass_JetBBEC1yearDown=svFitMass;
            svFitMass_JetEC2Up=svFitMass;
            svFitMass_JetEC2Down=svFitMass;
            svFitMass_JetEC2yearUp=svFitMass;
            svFitMass_JetEC2yearDown=svFitMass;
            svFitMass_JetFlavorQCDUp=svFitMass;
            svFitMass_JetFlavorQCDDown=svFitMass;
            svFitMass_JetHFUp=svFitMass;
            svFitMass_JetHFDown=svFitMass;
            svFitMass_JetHFyearUp=svFitMass;
            svFitMass_JetHFyearDown=svFitMass;
            svFitMass_JetRelativeBalUp=svFitMass;
            svFitMass_JetRelativeBalDown=svFitMass;
            svFitMass_JetRelativeSampleUp=svFitMass;
            svFitMass_JetRelativeSampleDown=svFitMass;
            svFitMass_JERUp=svFitMass;
            svFitMass_JERDown=svFitMass;

            svFitPhi_JetAbsoluteUp=svFitPhi;
            svFitPhi_JetAbsoluteDown=svFitPhi;
            svFitPhi_JetAbsoluteyearUp=svFitPhi;
            svFitPhi_JetAbsoluteyearDown=svFitPhi;
            svFitPhi_JetBBEC1Up=svFitPhi;
            svFitPhi_JetBBEC1Down=svFitPhi;
            svFitPhi_JetBBEC1yearUp=svFitPhi;
            svFitPhi_JetBBEC1yearDown=svFitPhi;
            svFitPhi_JetEC2Up=svFitPhi;
            svFitPhi_JetEC2Down=svFitPhi;
            svFitPhi_JetEC2yearUp=svFitPhi;
            svFitPhi_JetEC2yearDown=svFitPhi;
            svFitPhi_JetFlavorQCDUp=svFitPhi;
            svFitPhi_JetFlavorQCDDown=svFitPhi;
            svFitPhi_JetHFUp=svFitPhi;
            svFitPhi_JetHFDown=svFitPhi;
            svFitPhi_JetHFyearUp=svFitPhi;
            svFitPhi_JetHFyearDown=svFitPhi;
            svFitPhi_JetRelativeBalUp=svFitPhi;
            svFitPhi_JetRelativeBalDown=svFitPhi;
            svFitPhi_JetRelativeSampleUp=svFitPhi;
            svFitPhi_JetRelativeSampleDown=svFitPhi;
            svFitPhi_JERUp=svFitPhi;
            svFitPhi_JERDown=svFitPhi;

            svFitEta_JetAbsoluteUp=svFitEta;
            svFitEta_JetAbsoluteDown=svFitEta;
            svFitEta_JetAbsoluteyearUp=svFitEta;
            svFitEta_JetAbsoluteyearDown=svFitEta;
            svFitEta_JetBBEC1Up=svFitEta;
            svFitEta_JetBBEC1Down=svFitEta;
            svFitEta_JetBBEC1yearUp=svFitEta;
            svFitEta_JetBBEC1yearDown=svFitEta;
            svFitEta_JetEC2Up=svFitEta;
            svFitEta_JetEC2Down=svFitEta;
            svFitEta_JetEC2yearUp=svFitEta;
            svFitEta_JetEC2yearDown=svFitEta;
            svFitEta_JetFlavorQCDUp=svFitEta;
            svFitEta_JetFlavorQCDDown=svFitEta;
            svFitEta_JetHFUp=svFitEta;
            svFitEta_JetHFDown=svFitEta;
            svFitEta_JetHFyearUp=svFitEta;
            svFitEta_JetHFyearDown=svFitEta;
            svFitEta_JetRelativeBalUp=svFitEta;
            svFitEta_JetRelativeBalDown=svFitEta;
            svFitEta_JetRelativeSampleUp=svFitEta;
            svFitEta_JetRelativeSampleDown=svFitEta;
            svFitEta_JERUp=svFitEta;
            svFitEta_JERDown=svFitEta;

            svFitPt_JetAbsoluteUp=svFitPt;
            svFitPt_JetAbsoluteDown=svFitPt;
            svFitPt_JetAbsoluteyearUp=svFitPt;
            svFitPt_JetAbsoluteyearDown=svFitPt;
            svFitPt_JetBBEC1Up=svFitPt;
            svFitPt_JetBBEC1Down=svFitPt;
            svFitPt_JetBBEC1yearUp=svFitPt;
            svFitPt_JetBBEC1yearDown=svFitPt;
            svFitPt_JetEC2Up=svFitPt;
            svFitPt_JetEC2Down=svFitPt;
            svFitPt_JetEC2yearUp=svFitPt;
            svFitPt_JetEC2yearDown=svFitPt;
            svFitPt_JetFlavorQCDUp=svFitPt;
            svFitPt_JetFlavorQCDDown=svFitPt;
            svFitPt_JetHFUp=svFitPt;
            svFitPt_JetHFDown=svFitPt;
            svFitPt_JetHFyearUp=svFitPt;
            svFitPt_JetHFyearDown=svFitPt;
            svFitPt_JetRelativeBalUp=svFitPt;
            svFitPt_JetRelativeBalDown=svFitPt;
            svFitPt_JetRelativeSampleUp=svFitPt;
            svFitPt_JetRelativeSampleDown=svFitPt;
            svFitPt_JERUp=svFitPt;
            svFitPt_JERDown=svFitPt;
         }


	 if (doES>0){
	     float ES_UP_scale=pt1_scaleU/pt1;
	     if (doES==2){ // embedded
		if (fabs(eta1)<1.479) ES_UP_scale=1.005;
                if (fabs(eta1)>1.479) ES_UP_scale=1.0125;
	     }
	     double pt1_UP;
             pt1_UP = pt1 * ES_UP_scale;
             double metcorr_ex_UP, metcorr_ey_UP;
             double dx1_UP, dy1_UP;
             dx1_UP = pt1_UP * TMath::Cos( phi1 ) * (( 1. / ES_UP_scale ) - 1.);
             dy1_UP = pt1_UP * TMath::Sin( phi1 ) * (( 1. / ES_UP_scale ) - 1.);
             metcorr_ex_UP = measuredMETx + dx1_UP;
             metcorr_ey_UP = measuredMETy + dy1_UP;
             std::vector<classic_svFit::MeasuredTauLepton> measuredTauLeptonsEscaleUP;
             measuredTauLeptonsEscaleUP.push_back(classic_svFit::MeasuredTauLepton(classic_svFit::MeasuredTauLepton::kTauToElecDecay, pt1_UP, eta1,  phi1, 0.51100e-3));
             measuredTauLeptonsEscaleUP.push_back(classic_svFit::MeasuredTauLepton(classic_svFit::MeasuredTauLepton::kTauToHadDecay,  pt2, eta2, phi2,m2, decayMode2));
             runSVFit(measuredTauLeptonsEscaleUP, metcorr_ex_UP, metcorr_ey_UP, covMET, 0, svFitMass_ESCALEUP, svFitPt_ESCALEUP, svFitEta_ESCALEUP, svFitPhi_ESCALEUP);

             float ES_DOWN_scale=pt1_scaleD/pt1;
             if (doES==2){ // embedded
                if (fabs(eta1)<1.479) ES_DOWN_scale=0.995;
                if (fabs(eta1)>1.479) ES_DOWN_scale=0.9875;
             }
             double pt1_DOWN;
             pt1_DOWN = pt1 * ES_DOWN_scale;
             double metcorr_ex_DOWN, metcorr_ey_DOWN;
             double dx1_DOWN, dy1_DOWN;
             dx1_DOWN = pt1_DOWN * TMath::Cos( phi1 ) * (( 1. / ES_DOWN_scale ) - 1.);
             dy1_DOWN = pt1_DOWN * TMath::Sin( phi1 ) * (( 1. / ES_DOWN_scale ) - 1.);
             metcorr_ex_DOWN = measuredMETx + dx1_DOWN;
             metcorr_ey_DOWN = measuredMETy + dy1_DOWN;
             std::vector<classic_svFit::MeasuredTauLepton> measuredTauLeptonsEscaleDOWN;
             measuredTauLeptonsEscaleDOWN.push_back(classic_svFit::MeasuredTauLepton(classic_svFit::MeasuredTauLepton::kTauToElecDecay, pt1_DOWN, eta1,  phi1, 0.51100e-3));
             measuredTauLeptonsEscaleDOWN.push_back(classic_svFit::MeasuredTauLepton(classic_svFit::MeasuredTauLepton::kTauToHadDecay,  pt2, eta2, phi2,m2, decayMode2));
             runSVFit(measuredTauLeptonsEscaleDOWN, metcorr_ex_DOWN, metcorr_ey_DOWN, covMET, 0, svFitMass_ESCALEDOWN, svFitPt_ESCALEDOWN, svFitEta_ESCALEDOWN, svFitPhi_ESCALEDOWN);

             if (gen_match_2<=5){
                float ES_UP_scale=1.0; // this value is for jet -> tau fakes
		if (year==2016){
                    if (doES==2 && gen_match_2==5 && decayMode2==0) ES_UP_scale=1.0046; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==1) ES_UP_scale=1.0022; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==10) ES_UP_scale=1.0033; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==11) ES_UP_scale=1.0033; // for real taus in embedded
                    if (doES==1 && gen_match_2==5){ // for real taus in MC
                        int bin_low = hist_lowpt_2016->GetXaxis()->FindBin(decayMode2);
                        int bin_high = hist_highpt_2016->GetXaxis()->FindBin(decayMode2);
                        float cent = hist_lowpt_2016->GetBinContent(bin_low);
                        float error_low = hist_lowpt_2016->GetBinError(bin_low);
                        float error_high = hist_highpt_2016->GetBinError(bin_high);
                        if (pt2>=170) ES_UP_scale=1.0+error_high/cent;
                        else if (pt2<34) ES_UP_scale=1.0+error_low/cent;
                        else ES_UP_scale=1.0+(error_low+(error_high-error_low)*(pt2-34.)/(170.-34.))/cent;
                    }
		    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==0 && fabs(eta2)<1.5) ES_UP_scale=1.0+gfes_2016->GetErrorYhigh(0)/gfes_2016->GetY()[0]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==1 && fabs(eta2)<1.5) ES_UP_scale=1.0+gfes_2016->GetErrorYhigh(1)/gfes_2016->GetY()[1]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==0 && fabs(eta2)>1.5) ES_UP_scale=1.0+gfes_2016->GetErrorYhigh(2)/gfes_2016->GetY()[2]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==1 && fabs(eta2)>1.5) ES_UP_scale=1.0+gfes_2016->GetErrorYhigh(3)/gfes_2016->GetY()[3]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==2 or gen_match_2==4)) ES_UP_scale=1.01; // for mu->tauh fakes in MC
		}
		else if (year==2017){
                    if (doES==2 && gen_match_2==5 && decayMode2==0) ES_UP_scale=1.0041; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==1) ES_UP_scale=1.0052; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==10) ES_UP_scale=1.0044; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==11) ES_UP_scale=1.0044; // for real taus in embedded
                    if (doES==1 && gen_match_2==5){ // for real taus in MC
                        int bin_low = hist_lowpt_2017->GetXaxis()->FindBin(decayMode2);
                        int bin_high = hist_highpt_2017->GetXaxis()->FindBin(decayMode2);
                        float cent = hist_lowpt_2017->GetBinContent(bin_low);
                        float error_low = hist_lowpt_2017->GetBinError(bin_low);
                        float error_high = hist_highpt_2017->GetBinError(bin_high);
                        if (pt2>=170) ES_UP_scale=1.0+error_high/cent;
                        else if (pt2<34) ES_UP_scale=1.0+error_low/cent;
                        else ES_UP_scale=1.0+(error_low+(error_high-error_low)*(pt2-34.)/(170.-34.))/cent; 
                    }
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==0 && fabs(eta2)<1.5) ES_UP_scale=1.0+gfes_2017->GetErrorYhigh(0)/gfes_2017->GetY()[0]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==1 && fabs(eta2)<1.5) ES_UP_scale=1.0+gfes_2017->GetErrorYhigh(1)/gfes_2017->GetY()[1]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==0 && fabs(eta2)>1.5) ES_UP_scale=1.0+gfes_2017->GetErrorYhigh(2)/gfes_2017->GetY()[2]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==1 && fabs(eta2)>1.5) ES_UP_scale=1.0+gfes_2017->GetErrorYhigh(3)/gfes_2017->GetY()[3]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==2 or gen_match_2==4)) ES_UP_scale=1.01; // for mu->tauh fakes in MC
                }
		else if (year==2018){
                    if (doES==2 && gen_match_2==5 && decayMode2==0) ES_UP_scale=1.0039; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==1) ES_UP_scale=1.0037; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==10) ES_UP_scale=1.0032; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==11) ES_UP_scale=1.0032; // for real taus in embedded
                    if (doES==1 && gen_match_2==5){ // for real taus in MC
                        int bin_low = hist_lowpt_2018->GetXaxis()->FindBin(decayMode2);
                        int bin_high = hist_highpt_2018->GetXaxis()->FindBin(decayMode2);
                        float cent = hist_lowpt_2018->GetBinContent(bin_low);
                        float error_low = hist_lowpt_2018->GetBinError(bin_low);
                        float error_high = hist_highpt_2018->GetBinError(bin_high);
                        if (pt2>=170) ES_UP_scale=1.0+error_high/cent;
                        else if (pt2<34) ES_UP_scale=1.0+error_low/cent;
                        else ES_UP_scale=1.0+(error_low+(error_high-error_low)*(pt2-34.)/(170.-34.))/cent;
                    }
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==0 && fabs(eta2)<1.5) ES_UP_scale=1.0+gfes_2018->GetErrorYhigh(0)/gfes_2018->GetY()[0]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==1 && fabs(eta2)<1.5) ES_UP_scale=1.0+gfes_2018->GetErrorYhigh(1)/gfes_2018->GetY()[1]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==0 && fabs(eta2)>1.5) ES_UP_scale=1.0+gfes_2018->GetErrorYhigh(2)/gfes_2018->GetY()[2]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==1 && fabs(eta2)>1.5) ES_UP_scale=1.0+gfes_2018->GetErrorYhigh(3)/gfes_2018->GetY()[3]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==2 or gen_match_2==4)) ES_UP_scale=1.01; // for mu->tauh fakes in MC
                }
                double pt2_UP;
                double mass2_UP=m2;
                if (decayMode2!=0) mass2_UP = m2 * ES_UP_scale;
                pt2_UP = pt2 * ES_UP_scale;
                double metcorr_ex_UP, metcorr_ey_UP;
                double dx2_UP, dy2_UP;
                dx2_UP = pt2_UP * TMath::Cos( phi2 ) * (( 1. / ES_UP_scale ) - 1.);
                dy2_UP = pt2_UP * TMath::Sin( phi2 ) * (( 1. / ES_UP_scale ) - 1.);
                metcorr_ex_UP = measuredMETx + dx2_UP;
                metcorr_ey_UP = measuredMETy + dy2_UP;

                std::vector<classic_svFit::MeasuredTauLepton> measuredTauLeptonsUP;
                measuredTauLeptonsUP.push_back(classic_svFit::MeasuredTauLepton(classic_svFit::MeasuredTauLepton::kTauToElecDecay, pt1, eta1,  phi1, 0.51100e-3));
                measuredTauLeptonsUP.push_back(classic_svFit::MeasuredTauLepton(classic_svFit::MeasuredTauLepton::kTauToHadDecay,  pt2_UP, eta2, phi2,  mass2_UP, decayMode2));
               runSVFit(measuredTauLeptonsUP, metcorr_ex_UP, metcorr_ey_UP, covMET, 0, svFitMass_UP, svFitPt_UP, svFitEta_UP, svFitPhi_UP);

                float ES_DOWN_scale=1.0; 
                if (year==2016){
                    if (doES==2 && gen_match_2==5 && decayMode2==0) ES_DOWN_scale=0.9954; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==1) ES_DOWN_scale=0.9975; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==10) ES_DOWN_scale=0.9949; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==11) ES_DOWN_scale=0.9949; // for real taus in embedded
                    if (doES==1 && gen_match_2==5){ // for real taus in MC
                        int bin_low = hist_lowpt_2016->GetXaxis()->FindBin(decayMode2);
                        int bin_high = hist_highpt_2016->GetXaxis()->FindBin(decayMode2);
                        float cent = hist_lowpt_2016->GetBinContent(bin_low);
                        float error_low = hist_lowpt_2016->GetBinError(bin_low);
                        float error_high = hist_highpt_2016->GetBinError(bin_high);
                        if (pt2>=170) ES_DOWN_scale=1.0-error_high/cent;
                        else if (pt2<34) ES_DOWN_scale=1.0-error_low/cent;
                        else ES_DOWN_scale=1.0-(error_low+(error_high-error_low)*(pt2-34.)/(170.-34.))/cent;
		    }
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==0 && fabs(eta2)<1.5) ES_DOWN_scale=1.0-gfes_2016->GetErrorYlow(0)/gfes_2016->GetY()[0]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==1 && fabs(eta2)<1.5) ES_DOWN_scale=1.0-gfes_2016->GetErrorYlow(1)/gfes_2016->GetY()[1]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==0 && fabs(eta2)>1.5) ES_DOWN_scale=1.0-gfes_2016->GetErrorYlow(2)/gfes_2016->GetY()[2]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==1 && fabs(eta2)>1.5) ES_DOWN_scale=1.0-gfes_2016->GetErrorYlow(3)/gfes_2016->GetY()[3]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==2 or gen_match_2==4)) ES_DOWN_scale=0.99; // for mu->tauh fakes in MC
                }
                else if (year==2017){
                    if (doES==2 && gen_match_2==5 && decayMode2==0) ES_DOWN_scale=0.9958; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==1) ES_DOWN_scale=0.9979; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==10) ES_DOWN_scale=0.9954; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==11) ES_DOWN_scale=0.9954; // for real taus in embedded
                    if (doES==1 && gen_match_2==5){ // for real taus in MC
                        int bin_low = hist_lowpt_2017->GetXaxis()->FindBin(decayMode2);
                        int bin_high = hist_highpt_2017->GetXaxis()->FindBin(decayMode2);
                        float cent = hist_lowpt_2017->GetBinContent(bin_low);
                        float error_low = hist_lowpt_2017->GetBinError(bin_low);
                        float error_high = hist_highpt_2017->GetBinError(bin_high);
                        if (pt2>=170) ES_DOWN_scale=1.0-error_high/cent;
                        else if (pt2<34) ES_DOWN_scale=1.0-error_low/cent;
                        else ES_DOWN_scale=1.0-(error_low+(error_high-error_low)*(pt2-34.)/(170.-34.))/cent;
                    }
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==0 && fabs(eta2)<1.5) ES_DOWN_scale=1.0-gfes_2017->GetErrorYlow(0)/gfes_2017->GetY()[0]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==1 && fabs(eta2)<1.5) ES_DOWN_scale=1.0-gfes_2017->GetErrorYlow(1)/gfes_2017->GetY()[1]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==0 && fabs(eta2)>1.5) ES_DOWN_scale=1.0-gfes_2017->GetErrorYlow(2)/gfes_2017->GetY()[2]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==1 && fabs(eta2)>1.5) ES_DOWN_scale=1.0-gfes_2017->GetErrorYlow(3)/gfes_2017->GetY()[3]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==2 or gen_match_2==4)) ES_DOWN_scale=0.99; // for mu->tauh fakes in MC
                }
                else if (year==2018){
                    if (doES==2 && gen_match_2==5 && decayMode2==0) ES_DOWN_scale=0.9961; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==1) ES_DOWN_scale=0.9969; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==10) ES_DOWN_scale=0.9968; // for real taus in embedded
                    else if (doES==2 && gen_match_2==5 && decayMode2==11) ES_DOWN_scale=0.9968; // for real taus in embedded
                    if (doES==1 && gen_match_2==5){ // for real taus in MC
                        int bin_low = hist_lowpt_2018->GetXaxis()->FindBin(decayMode2);
                        int bin_high = hist_highpt_2018->GetXaxis()->FindBin(decayMode2);
                        float cent = hist_lowpt_2018->GetBinContent(bin_low);
                        float error_low = hist_lowpt_2018->GetBinError(bin_low);
                        float error_high = hist_highpt_2018->GetBinError(bin_high);
                        if (pt2>=170) ES_DOWN_scale=1.0-error_high/cent;
                        else if (pt2<34) ES_DOWN_scale=1.0-error_low/cent;
                        else ES_DOWN_scale=1.0-(error_low+(error_high-error_low)*(pt2-34.)/(170.-34.))/cent;
                    }
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==0 && fabs(eta2)<1.5) ES_DOWN_scale=1.0-gfes_2018->GetErrorYlow(0)/gfes_2018->GetY()[0]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==1 && fabs(eta2)<1.5) ES_DOWN_scale=1.0-gfes_2018->GetErrorYlow(1)/gfes_2018->GetY()[1]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==0 && fabs(eta2)>1.5) ES_DOWN_scale=1.0-gfes_2018->GetErrorYlow(2)/gfes_2018->GetY()[2]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==1 or gen_match_2==3) && decayMode2==1 && fabs(eta2)>1.5) ES_DOWN_scale=1.0-gfes_2018->GetErrorYlow(3)/gfes_2018->GetY()[3]; // for e->tauh fakes in MC
                    else if (doES==1 && (gen_match_2==2 or gen_match_2==4)) ES_DOWN_scale=0.99; // for mu->tauh fakes in MC
                }
                double pt2_DOWN;
                double mass2_DOWN = m2;
                pt2_DOWN = pt2 * ES_DOWN_scale;
                if (decayMode2!=0) mass2_DOWN = m2 * ES_DOWN_scale;
                double metcorr_ex_DOWN, metcorr_ey_DOWN;
                double dx2_DOWN, dy2_DOWN;
                dx2_DOWN = pt2_DOWN * TMath::Cos( phi2 ) * (( 1. / ES_DOWN_scale ) - 1.);
                dy2_DOWN = pt2_DOWN * TMath::Sin( phi2 ) * (( 1. / ES_DOWN_scale ) - 1.);
                metcorr_ex_DOWN = measuredMETx + dx2_DOWN;
                metcorr_ey_DOWN = measuredMETy + dy2_DOWN;

                std::vector<classic_svFit::MeasuredTauLepton> measuredTauLeptonsDOWN;
                measuredTauLeptonsDOWN.push_back(classic_svFit::MeasuredTauLepton(classic_svFit::MeasuredTauLepton::kTauToElecDecay, pt1, eta1,  phi1, 0.51100e-3));
                measuredTauLeptonsDOWN.push_back(classic_svFit::MeasuredTauLepton(classic_svFit::MeasuredTauLepton::kTauToHadDecay,  pt2_DOWN, eta2, phi2,  mass2_DOWN, decayMode2));
               runSVFit(measuredTauLeptonsDOWN, metcorr_ex_DOWN, metcorr_ey_DOWN, covMET, 0, svFitMass_DOWN, svFitPt_DOWN, svFitEta_DOWN, svFitPhi_DOWN);

	    }
	    else{
	       svFitMass_DOWN=svFitMass;
               svFitMass_UP=svFitMass;
               svFitPt_DOWN=svFitPt;
               svFitPt_UP=svFitPt;
               svFitEta_DOWN=svFitEta;
               svFitEta_UP=svFitEta;
               svFitPhi_DOWN=svFitPhi;
               svFitPhi_UP=svFitPhi;
	    }
	 }

         newBranch1->Fill();
         newBranch1U->Fill();
         newBranch1D->Fill();
         newBranch1UU->Fill();
         newBranch1UD->Fill();
         newBranch1ESCU->Fill();
         newBranch1ESCD->Fill();
         newBranch1ResponseU->Fill();
         newBranch1ResponseD->Fill();
         newBranch1ResolutionU->Fill();
         newBranch1ResolutionD->Fill();
         newBranch1JetAbsoluteU->Fill();
         newBranch1JetAbsoluteD->Fill();
         newBranch1JetAbsoluteyearU->Fill();
         newBranch1JetAbsoluteyearD->Fill();
         newBranch1JetBBEC1U->Fill();
         newBranch1JetBBEC1D->Fill();
         newBranch1JetBBEC1yearU->Fill();
         newBranch1JetBBEC1yearD->Fill();
         newBranch1JetEC2U->Fill();
         newBranch1JetEC2D->Fill();
         newBranch1JetEC2yearU->Fill();
         newBranch1JetEC2yearD->Fill();
         newBranch1JetFlavorQCDU->Fill();
         newBranch1JetFlavorQCDD->Fill();
         newBranch1JetHFU->Fill();
         newBranch1JetHFD->Fill();
         newBranch1JetHFyearU->Fill();
         newBranch1JetHFyearD->Fill();
         newBranch1JetRelativeBalU->Fill();
         newBranch1JetRelativeBalD->Fill();
         newBranch1JetRelativeSampleU->Fill();
         newBranch1JetRelativeSampleD->Fill();
         newBranch1JERU->Fill();
         newBranch1JERD->Fill();

         newBranchPt1->Fill();
         newBranchPt1U->Fill();
         newBranchPt1D->Fill();
         newBranchPt1UU->Fill();
         newBranchPt1UD->Fill();
         newBranchPt1ESCU->Fill();
         newBranchPt1ESCD->Fill();
         newBranchPt1ResponseU->Fill();
         newBranchPt1ResponseD->Fill();
         newBranchPt1ResolutionU->Fill();
         newBranchPt1ResolutionD->Fill();
         newBranchPt1JetAbsoluteU->Fill();
         newBranchPt1JetAbsoluteD->Fill();
         newBranchPt1JetAbsoluteyearU->Fill();
         newBranchPt1JetAbsoluteyearD->Fill();
         newBranchPt1JetBBEC1U->Fill();
         newBranchPt1JetBBEC1D->Fill();
         newBranchPt1JetBBEC1yearU->Fill();
         newBranchPt1JetBBEC1yearD->Fill();
         newBranchPt1JetEC2U->Fill();
         newBranchPt1JetEC2D->Fill();
         newBranchPt1JetEC2yearU->Fill();
         newBranchPt1JetEC2yearD->Fill();
         newBranchPt1JetFlavorQCDU->Fill();
         newBranchPt1JetFlavorQCDD->Fill();
         newBranchPt1JetHFU->Fill();
         newBranchPt1JetHFD->Fill();
         newBranchPt1JetHFyearU->Fill();
         newBranchPt1JetHFyearD->Fill();
         newBranchPt1JetRelativeBalU->Fill();
         newBranchPt1JetRelativeBalD->Fill();
         newBranchPt1JetRelativeSampleU->Fill();
         newBranchPt1JetRelativeSampleD->Fill();
         newBranchPt1JERU->Fill();
         newBranchPt1JERD->Fill();

         newBranchEta1->Fill();
         newBranchEta1U->Fill();
         newBranchEta1D->Fill();
         newBranchEta1UU->Fill();
         newBranchEta1UD->Fill();
         newBranchEta1ESCU->Fill();
         newBranchEta1ESCD->Fill();
         newBranchEta1ResponseU->Fill();
         newBranchEta1ResponseD->Fill();
         newBranchEta1ResolutionU->Fill();
         newBranchEta1ResolutionD->Fill();
         newBranchEta1JetAbsoluteU->Fill();
         newBranchEta1JetAbsoluteD->Fill();
         newBranchEta1JetAbsoluteyearU->Fill();
         newBranchEta1JetAbsoluteyearD->Fill();
         newBranchEta1JetBBEC1U->Fill();
         newBranchEta1JetBBEC1D->Fill();
         newBranchEta1JetBBEC1yearU->Fill();
         newBranchEta1JetBBEC1yearD->Fill();
         newBranchEta1JetEC2U->Fill();
         newBranchEta1JetEC2D->Fill();
         newBranchEta1JetEC2yearU->Fill();
         newBranchEta1JetEC2yearD->Fill();
         newBranchEta1JetFlavorQCDU->Fill();
         newBranchEta1JetFlavorQCDD->Fill();
         newBranchEta1JetHFU->Fill();
         newBranchEta1JetHFD->Fill();
         newBranchEta1JetHFyearU->Fill();
         newBranchEta1JetHFyearD->Fill();
         newBranchEta1JetRelativeBalU->Fill();
         newBranchEta1JetRelativeBalD->Fill();
         newBranchEta1JetRelativeSampleU->Fill();
         newBranchEta1JetRelativeSampleD->Fill();
         newBranchEta1JERU->Fill();
         newBranchEta1JERD->Fill();

         newBranchPhi1->Fill();
         newBranchPhi1U->Fill();
         newBranchPhi1D->Fill();
         newBranchPhi1UU->Fill();
         newBranchPhi1UD->Fill();
         newBranchPhi1ESCU->Fill();
         newBranchPhi1ESCD->Fill();
         newBranchPhi1ResponseU->Fill();
         newBranchPhi1ResponseD->Fill();
         newBranchPhi1ResolutionU->Fill();
         newBranchPhi1ResolutionD->Fill();
         newBranchPhi1JetAbsoluteU->Fill();
         newBranchPhi1JetAbsoluteD->Fill();
         newBranchPhi1JetAbsoluteyearU->Fill();
         newBranchPhi1JetAbsoluteyearD->Fill();
         newBranchPhi1JetBBEC1U->Fill();
         newBranchPhi1JetBBEC1D->Fill();
         newBranchPhi1JetBBEC1yearU->Fill();
         newBranchPhi1JetBBEC1yearD->Fill();
         newBranchPhi1JetEC2U->Fill();
         newBranchPhi1JetEC2D->Fill();
         newBranchPhi1JetEC2yearU->Fill();
         newBranchPhi1JetEC2yearD->Fill();
         newBranchPhi1JetFlavorQCDU->Fill();
         newBranchPhi1JetFlavorQCDD->Fill();
         newBranchPhi1JetHFU->Fill();
         newBranchPhi1JetHFD->Fill();
         newBranchPhi1JetHFyearU->Fill();
         newBranchPhi1JetHFyearD->Fill();
         newBranchPhi1JetRelativeBalU->Fill();
         newBranchPhi1JetRelativeBalD->Fill();
         newBranchPhi1JetRelativeSampleU->Fill();
         newBranchPhi1JetRelativeSampleD->Fill();
         newBranchPhi1JERU->Fill();
         newBranchPhi1JERD->Fill();

    }
      dir->cd();
      t->Write("",TObject::kOverwrite);
      strcpy(TreeToUse,stringA) ;

  }
 }
}

void runSVFit(std::vector<classic_svFit::MeasuredTauLepton> & measuredTauLeptons, double measuredMETx, double measuredMETy, TMatrixD &covMET, float num, float &svFitMass, float& svFitPt, float &svFitEta, float &svFitPhi){
  FastMTT aFastMTTAlgo;
  aFastMTTAlgo.run(measuredTauLeptons,measuredMETx,measuredMETy,covMET);
  LorentzVector ttP4 = aFastMTTAlgo.getBestP4();
  svFitMass = ttP4.M(); // return value is in units of GeV
  svFitPt = ttP4.Pt();
  svFitEta = ttP4.Eta();
  svFitPhi = ttP4.Phi();
  //std::cout << "found mass = " << svFitMass << std::endl;

}

void CopyDir(TDirectory *source, optutl::CommandLineParser parser) {
  TDirectory *savdir = gDirectory;
  TDirectory *adir = savdir; 
  if(source->GetName()!=parser.stringValue("inputFile")){
    adir = savdir->mkdir(source->GetName());
    std::cout<<"Source name is not outputfile name"<<std::endl;
    adir->cd();    
  }
  else{
    adir->cd();    
  }

  //loop on all entries of this directory
  TKey *key;
  TIter nextkey(source->GetListOfKeys());
  while ((key = (TKey*)nextkey())) {
    std::cout<<"My key is: "<<key->GetName()<<std::endl;
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname);
    if (!cl) continue;
    if (cl->InheritsFrom(TDirectory::Class())) {
      source->cd(key->GetName());
      TDirectory *subdir = gDirectory;
      adir->cd();
      CopyDir(subdir,parser);
      adir->cd();
    } else if (cl->InheritsFrom(TTree::Class())) {
      TTree *T = (TTree*)source->Get(key->GetName());
      adir->cd();
      TTree *newT = T->CloneTree(-1,"fast");
      newT->Write();
    } else {
      source->cd();
      TObject *obj = key->ReadObj();
      adir->cd();
      obj->Write();
      delete obj;
    }
  }
  adir->SaveSelf(kTRUE);
  savdir->cd();
}
void CopyFile(const char *fname, optutl::CommandLineParser parser) {
  //Copy all objects and subdirs of file fname as a subdir of the current directory
  TDirectory *target = gDirectory;
  TFile *f = TFile::Open(fname);
  if (!f || f->IsZombie()) {
    printf("Cannot copy file: %s\n",fname);
    target->cd();
    return;
  }
  target->cd();
  CopyDir(f,parser);
  delete f;
  target->cd();
}
void copyFiles( optutl::CommandLineParser parser, TFile* fOld, TFile* fNew) 
{
  //prepare files to be copied
  if(gSystem->AccessPathName(parser.stringValue("inputFile").c_str())) {
    gSystem->CopyFile("hsimple.root", parser.stringValue("inputFile").c_str());
  }

  fNew->cd();
  CopyFile(parser.stringValue("inputFile").c_str(),parser);
  fNew->ls();
  fNew->Close();

}

