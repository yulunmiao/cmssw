import os
import ROOT
import pandas as pd
import numpy as np
import re

basedir='/eos/cms/store/group/dpg_hgcal/comm_hgcal/2023/CMSSW/ReReco_Sep26/'
pedestal_run=f'{basedir}/Run1695716961/d1643b06-5cc3-11ee-bee0-fa163e17e03d'
#mip_run=f'{basedir}/Run1695563673/c6adb2e6-5cc3-11ee-bee0-fa163e17e03d'
#mip_run=f'{basedir}/Run1695564190/c713c360-5cc3-11ee-bee0-fa163e17e03d'
mip_run=f'{basedir}/Run1695564694/c764c29c-5cc3-11ee-bee0-fa163e17e03d'
#mip_run=f'{basedir}/Run1695565177/c7d12c48-5cc3-11ee-bee0-fa163e17e03d'
#mip_run=f'{basedir}Run1695565613/c82c5db6-5cc3-11ee-bee0-fa163e17e03d'
selection_metric='NoiseRatio'

def findCandidateChannels(ped,mip,metric='NoiseRatio'):

    """ reads the DQM outputs and selects the channel for which std(mip)/std(ped) has increased the most """


    def _setChannelModule(df):

        """helper function to uniformize channel"""
        
        #string to integer
        df['Channel'] = df['Channel'].apply(lambda x: int(x, 16))

        #in case it's not a DTH run the modules come as two separate FEDs
        df['Channel'] = np.where(df['Channel']>0x40000,
                                     df['Channel']-0x40000+0x400,
                                     df['Channel'])

        #assign a module number fo grouping
        df['Module'] = np.where((df['Channel']<0x400),1,2)
        df=df.astype({'Module':int})
        
        return df
    
    
    #merge mip and pedestal summaries
    columns=['Channel','Pedestal','Noise']
    ped_df=pd.read_csv(ped,sep='\s+',header='infer')[columns]
    ped_df=_setChannelModule(ped_df)
    mip_df=pd.read_csv(mip,sep='\s+',header='infer')[columns]
    mip_df=_setChannelModule(mip_df)
    df=mip_df.merge(ped_df,on='Channel',suffixes=('', '_ped'))

    #remove dead channels
    mask=(df['Noise']==0) | (df['Noise_ped']==0)
    df=df[~mask].copy()
    df['NoiseRatio'] = df['Noise']/df['Noise_ped']

    #select channels of interest
    max_var_channels = df.groupby(['Module'])[metric].idxmax().values
    mip_spot_channels = df.iloc[max_var_channels][['Channel','Module','Pedestal_ped','NoiseRatio','Noise']]
    print(f'Identified two possible candidates for MIP spots by {metric}')
    print(mip_spot_channels.head())
    
    return mip_spot_channels


mip_spot_channels=findCandidateChannels(ped=f'{pedestal_run}/calibs/level0_calib_params.txt',
                                        mip=f'{mip_run}/calibs/level0_calib_params.txt',
                                        metric=selection_metric)
chlist=mip_spot_channels['Channel'].values
pedlist=mip_spot_channels['Pedestal_ped'].values
modlist=mip_spot_channels['Module'].values

#select in NANOAOD, subtract pedestals and draw the MIP peak
flist=[os.path.join(mip_run,f) for f in os.listdir(mip_run) if 'NANO' in f]

ROOT.ROOT.EnableImplicitMT()

df=ROOT.RDataFrame('Events',flist)

chlist=mip_spot_channels['Channel'].values
chsel = ' || '. join( [f'HGC_eleid=={ch}' for ch in chlist] )
df=df.Define('goodch',chsel).Filter('Sum(goodch)==2')

pedlist=mip_spot_channels['Pedestal_ped'].values
histos=[]
for i in range(len(chlist)):
    pedval=pedlist[i]
    h = df.Define(f'goodch_ADC{i}',f'HGC_adc[goodch][{i}]-{pedval}') \
          .Histo1D( (f'adc{i}',f'Module {modlist[i]};ADC counts;Events',50,0,50), f'goodch_ADC{i}')
    histos.append(h.GetPtr())

ROOT.ROOT.DisableImplicitMT()

    
#plot
ROOT.gStyle.SetOptStat(0); 
ROOT.gStyle.SetTextFont(42)
ROOT.gStyle.SetOptTitle(0)
c = ROOT.TCanvas("c", "c", 800, 800)
c.SetTopMargin(0.05)
c.SetLeftMargin(0.12)
c.SetRightMargin(0.05)
frame=histos[0].Clone('frame')
frame.Reset('ICE')
frame.Draw()
run_number=re.findall('Run(\d+)',mip_run)[0]
leg=ROOT.TLegend(0.6,0.7,0.95,0.95,f'Run {run_number}')
leg.SetFillStyle(0)
leg.SetBorderSize(0)
for i,h in enumerate(histos):
    h.Draw('histsame')
    h.SetLineWidth(2)
    h.SetLineColor(i+1)
    leg.AddEntry(h,h.GetTitle(),'l')
frame.GetYaxis().SetRangeUser(1,1e6)
c.SetLogy()
leg.Draw()
c.Modified()
c.Update()
c.SaveAs(f'/eos/user/p/psilva/www/HGCAL/NanoAOD/mip_{run_number}_{selection_metric}.png')
