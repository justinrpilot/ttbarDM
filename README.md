ttbarDM
=======

Analysis framework for tt+DM

----------------------------

cmsrel CMSSW_7_1_0
cd CMSSW_7_1_0/src  
cmsenv  
git clone https://github.com/justinrpilot/ttbarDM.git

for running
- on 71x sample
  cmsRun topplusdmanaEDMntuples_cfg.py maxEvts=100 sample="file:/afs/cern.ch/user/d/decosa/public/forTTDMteam/patTuple_tlbsm_train_tlbsm_71x_v1.root" version="71" &
- on 53x sample
  cmsRun topplusdmanaEDMntuples_cfg.py maxEvts=100 sample="file:/afs/cern.ch/user/d/decosa/public/forTTDMteam/tlbsm_53x_v3_mc_10_1_qPV.root" version="53" &