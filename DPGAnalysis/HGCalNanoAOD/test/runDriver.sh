cmsDriver.py NANO \
    -s USER:DPGAnalysis/HGCalNanoAOD/hgcRecHits_cff.hgcRecHitsTask \
    --datatier NANOAOD \
    --eventcontent NANOAOD \
    --filein file:/eos/user/d/dalfonso/HGCALtbeam/singleMu_pt10/step3.root \
    --fileout hgcalnano_testcmsdriver.root \
    -n -1 \
    --nThreads 4 \
    --conditions auto:phase2_realistic_T21 \
    --geometry Extended2026D95 \
    --era Phase2C17I13M9 \
    --python_filename testcmsdriver_cfg.py \
    --no_exec
cmsRun testcmsdriver_cfg.py

$CMSSW_BASE/src/PhysicsTools/NanoAOD/test/inspectNanoFile.py hgcalnano_testcmsdriver.root -j hgcalnano.json -s hgcalnano_report.html -d hgcalnano_doc.html
