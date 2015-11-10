
// example of post-processor, which is called once at the analysis end

#include <stdio.h>

#include "TTree.h"
#include "TH1.h"

#include "base/EventProc.h"
#include "base/Event.h"
#include "hadaq/TdcSubEvent.h"
#include "hadaq/TrbProcessor.h"
#include "hadaq/HldProcessor.h"
#include "hadaq/TdcProcessor.h"


class PostProcessor : public base::EventProc {
public:

  hadaq::HldProcessor* fHLD;
  
  base::H2handle  hCorr;  //!< correlation between mean and rms
  
  PostProcessor(hadaq::HldProcessor* hld) :
    base::EventProc("PostProcessor"),
    fHLD(hld)
  {
    hCorr = MakeH2("Corr","Correlation between mean and rms", 150, -10., 20., 30, 0., 0.03, "mean, ns;rms, ns");
  }
  
  virtual void UserPostLoop()
  {
    printf("UserPostLoop hld = %p\n", fHLD);
    if (fHLD==0) return;

    for (unsigned ntdc=0x0940;ntdc<0x0943;ntdc++) {
    
      hadaq::TdcProcessor* tdc = fHLD->FindTDC(ntdc);
      if (tdc==0) continue;

      for (unsigned  nch=0;nch<tdc->NumChannels(); nch++) {
	TH1* hist = (TH1*) tdc->GetChannelRefHist(nch);
	
	if (hist==0) continue;
	
	double mean = hist->GetMean();
	double rms = hist->GetRMS();
	
	FillH2(hCorr, mean, rms);
	// tdc->ClearChannelRefHist(nch);

	hist = (TH1*) tdc->GetChannelTotHist(nch);
	
	if (hist==0) continue;
	
	double tot_mean = hist->GetMean();
	double tot_rms  = hist->GetRMS();
	
	printf("  %s ch %u mean:%5.3f rms:%5.3f tot_mean:%5.3f tot_rms:%5.3f\n", tdc->GetName(), nch, mean, rms*1000, tot_mean, tot_rms*1000);
	//printf("  %s ch %u rms:%5.3f\n", tdc->GetName(), nch, rms);
      }
      
    }
    
  }
};


void second()
{
  hadaq::HldProcessor* hld = dynamic_cast<hadaq::HldProcessor*> (base::ProcMgr::instance()->FindProc("HLD"));
  
  // processor used only to invoke operation at the end of analysis
  new PostProcessor(hld);
  
}
