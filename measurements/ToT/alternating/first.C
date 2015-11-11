#include "Riostream.h"

void first()
{

   int chNumber = 33;

   base::ProcMgr::instance()->SetRawAnalysis(true);

   // this limits used for liner calibrations when nothing else is available
   hadaq::TdcMessage::SetFineLimits(31, 491);

   // default bins number for fine-counter histos and TOT range
   hadaq::TdcProcessor::SetDefaults(600, 100);

   // default channel numbers and edges mask
   hadaq::TrbProcessor::SetDefaults(chNumber, 0x2);
   //1: only leading edge 
   //2: leading/trailing individual
   //3: statistics from leading used 
   //4: statistics merged

   hadaq::HldProcessor* hld = new hadaq::HldProcessor();

   // About time calibration - there are two possibilities
   // 1) automatic calibration after N hits in every enabled channel.
   //     Just use SetAutoCalibrations method for this
   // 2) generate calibration on base of provided data and than use it later statically for analysis
   //     Than one makes special run with SetWriteCalibrations() enabled.
   //     Later one reuse such calibrations enabling only LoadCalibrations() call

   // hadaq::TrbProcessor* trb3_1 = new hadaq::TrbProcessor(0xC210, hld);
   // trb3_1->SetHadaqHUBId(0x8210);
   // //trb3_1->SetPrintRawData(true);
   // trb3_1->SetPrintRawData(false);
   // trb3_1->SetPrintErrors(1);
   // trb3_1->SetHistFilling(4);
   // trb3_1->SetCrossProcess(true);
   // trb3_1->CreateTDC(0x0210, 0x0cc0);
   // // enable automatic calibration, specify required number of hits in each channel
   // trb3_1->SetAutoCalibrations(200000);
   // // calculate and write static calibration at the end of the run
   // //trb3_1->SetWriteCalibrations("calibrations/cal_");
   // // load static calibration at the beginning of the run
   // //trb3_1->LoadCalibrations("calibrations/cal_");

   // hadaq::TrbProcessor* trb3_2 = new hadaq::TrbProcessor(0x8960, hld);
   // //trb3_2->SetPrintRawData(true);
   // trb3_2->SetPrintRawData(false);
   // trb3_2->SetPrintErrors(1);
   // trb3_2->SetHistFilling(4);
   // trb3_2->SetCrossProcess(true);
   // trb3_2->CreateTDC(0x0960, 0x0961, 0x0962, 0x0963);
   // trb3_2->SetAutoCalibrations(200000);
   // //trb3_2->SetWriteCalibrations("calibrations/cal_");
   // //trb3_2->LoadCalibrations("calibrations/cal_");

   hadaq::TrbProcessor* trb3_3 = new hadaq::TrbProcessor(0x8940, hld);
   //trb3_3->SetPrintRawData(true);
   trb3_3->SetPrintRawData(false);
   trb3_3->SetPrintErrors(1);
   trb3_3->SetHistFilling(4);
   trb3_3->SetCrossProcess(true);
   trb3_3->CreateTDC(0x0940, 0x0941, 0x0942);
   //trb3_3->CreateTDC(0x0942);
   //trb3_3->SetAutoCalibrations(200000);
   //trb3_3->SetWriteCalibrations("calibrations/cal_");
   trb3_3->LoadCalibrations("calibrations/cal_");


   // this is array with available TDCs ids
   int tdcmap[3] = {0x0940, 0x0941, 0x0942};

   // TDC subevent header id

   for (int cnt=0;cnt<3;cnt++) {

     hadaq::TdcProcessor* tdc = hld->FindTDC(tdcmap[cnt]);
     if (tdc==0) continue;
     tdc->SetCalibrTrigger(0x1);

     // ToT alternating
     for (int i=1;i<chNumber;i=i+2) {
       tdc->SetRefChannel(i, 1, 0xffff, 20000, -10.,  10., true);
       tdc->SetRefChannel(i+1, i, 0xffff, 20000, -20.,  20., true);
     }


     // ToT stretcher
     // for (int i=1;i<chNumber;i=i+1) {
     //   tdc->SetRefChannel(i, 1, 0xffff, 40000, -20.,  20., true);
     // }
     //tdc->SetRefChannel(0, 0, 0x0010, 40000, -20.,  20., true);



     // this is example how to specify conditional print
     // tdc->EnableRefCondPrint(ch, left, right, #ofprints);
     //tdc->EnableRefCondPrint(0, -20., 20., 1000); //41081.583

     // if (cnt==3){
     //   for (int n=1;n<chNumber;n=n+2){
     // 	 tdc->SetRefChannel(n, 1, 0x7c00c, 30000, -15., 15., true);
     //   }
     // }

     // specify reference channel from other TDC
     // if (cnt > 0) {
     //   tdc->SetRefChannel(0, 0, 0xC004, 20000, -10., 10., true);
     //   for (int n=1;n<chNumber;n=n+2){
     // 	 //tdc->SetRefChannel(n, n, 0xC004, 20000, -10., 10., true);
     // 	 tdc->SetRefChannel(n, n, 0x7C004, 20000, -10., 10., true);
     // 	 //tdc->SetRefChannel(n+1, n, 0xffff, 20000,   5., 25., true);
     //   }
     // }
     
     
     
     // for old FPGA code one should have epoch for each hit, no longer necessary
     // tdc->SetEveryEpoch(true);
     
     // When enabled, time of last hit will be used for reference calculations
     // tdc->SetUseLastHit(true);
     
     
#ifdef __GO4ANAMACRO__
     int numx = 1;
     int numy = 1;
     while ((numx * numy) < tdc->NumChannels()) {
       if (numx==numy) numy++; else numx++;
     }
     
     TGo4Picture** pic = new TGo4Picture*[tdc->GetNumHist()];
     int* piccnt = new int[tdc->GetNumHist()];
     for (int k=0;k<tdc->GetNumHist();k++) {
       pic[k] = new TGo4Picture(Form("%s_%s", tdc->GetName(), tdc->GetHistName(k)), Form("All %s", tdc->GetHistName(k)));
       pic[k]->SetDivision(numy,numx);
       piccnt[k] = 0;
     }
     for (int nch=0;nch<tdc->NumChannels();nch++) {
       int x(0), y(0);
       if (nch==0) { x = numx-1; y = numy-1; }
       else { x = (nch - 1) % numx; y = (nch-1) / numx; }
       for (int k=0;k<tdc->GetNumHist();k++) {
	 TObject* obj = (TObject*) tdc->GetHist(nch, k);
	 if (obj) {
	   piccnt[k]++;
	   pic[k]->Pic(y,x)->AddObject(obj);
	 }
       }
     }
     for (int k=0;k<tdc->GetNumHist();k++) {
       if (piccnt[k] > 0) go4->AddPicture(pic[k]);
       else delete pic[k];
     }
     delete[] pic;
     delete[] piccnt;
     
#endif
   }

   // This is a method to regularly invoke macro, where arbitrary action can be performed
   // One could specify period in seconds or function will be called for every event processed

   //   new THookProc("my_hook();", 2.5);

}



// this is example of hook function
// here one gets access to all tdc processors and obtains Mean and RMS
// value on the first channel for each TDC and prints on the display
int hook_counter=0;
int width_counter=0;

void my_hook()
{
  hadaq::HldProcessor* hld = base::ProcMgr::instance()->FindProc("HLD");

  cout << Form("hook counter: %d", hook_counter) << endl;





  // Calibraton Time //  
  if (width_counter==0)
    {
      // int chId = 5;
      // gSystem->Exec(Form("echo -e 'asdf \tasdf sadf sadf as f' >> ~/git_cahitugur/trb3/measurements/ToT/pulseWidth_scan.txt"));

      // gSystem->Exec(Form("echo -e '%5.3f \t%d' >> ~/git_cahitugur/trb3/measurements/ToT/pulseWidth_scan.txt", width_counter*1.667, chId));


      if (hook_counter==15)
	{
	  width_counter++;
	  hook_counter = 0;
	  continue;
	}
      hook_counter++;
      continue;
    }
  ////////////////////////////////////

  // Pulser Settings //
  if (hook_counter==0) 
    {
      printf("Set pulser\n");
      gSystem->Exec(Form("perl ~/git_cahitugur/scripts/telnet_command_send.pl -pt 0x%x -pr 0x1fff -st 0x11", width_counter));
    }

  else if(hook_counter == 10)
    {
      width_counter++;
      hook_counter = 0;
      continue;
    }
  hook_counter++;
  //////////////////////////////////////

  // Histogram generation //
  int tdcmap[2] = { 0xC000, 0xC008 };

  if(hook_counter == 2)
    {
      cout << Form("Clear histograms\n") << endl;
      for (int cnt=0;cnt<2;cnt++) {
	
	hadaq::TdcProcessor* tdc = hld ? hld->FindTDC(tdcmap[cnt]) : 0;
	if (tdc==0) { printf("DID NOT FOUND TDC\n"); return; }
	for (int chId=1;chId<chNumber;chId=chId+1){
	  //      tdc_1->ClearChannelRefHist(chId);
	  tdc->ClearHist(chId, 5);
	}
      }
    }
  else if(hook_counter == 10)
    {
      gSystem->Exec(Form("echo -e '# pulseWidth(ns) \tTDC \tCHANNEL \tMEAN(ns) \tRMS(ps)' >> ~/git_cahitugur/trb3/measurements/ProgrammableOscillator/pulseWidth_scan.txt"));
      for (int cnt=0;cnt<2;cnt++) {
	
	hadaq::TdcProcessor* tdc = hld ? hld->FindTDC(tdcmap[cnt]) : 0;
	if (tdc==0) { printf("DID NOT FOUND TDC\n"); return; }
	
	for (int chId=1;chId<chNumber;chId=chId+1){
	  //  TH1* hist = (TH1*) tdc_1->GetChannelRefHist(chId); // argument is the channel number
	  TH1* tot = (TH1*) tdc->GetHist(chId,5); // arguments are the channel number and the ToT histogram number
	  gSystem->Exec(Form("echo -e '%5.3f \t\t%x \t%d \t\t%5.3f \t\t%5.4f' >> ~/git_cahitugur/trb3/measurements/ProgrammableOscillator/pulseWidth_scan.txt", width_counter*1.667, tdc->GetID(), chId, tot->GetMean(), tot->GetRMS()*1000 ));
	}
      }
    }
 }





  // int tdcmap[2] = { 0xC000, 0xC00C };
  // int chId=2;

  // hadaq::HldProcessor* hld = base::ProcMgr::instance()->FindProc("HLD");
  // hadaq::TdcProcessor* tdc_1 = hld ? hld->FindTDC(tdcmap[0]) : 0;
  // if (tdc_1==0) { printf("DID NOT FOUND TDC\n"); return; }
  // //cout << Form("FOUND TDC id:%x", tdc_1->GetID()) << endl;
  // hadaq::TdcProcessor* tdc_2 = hld ? hld->FindTDC(tdcmap[1]) : 0;
  // if (tdc_2==0) { printf("DID NOT FOUND TDC\n"); return; }
  // //cout << Form("FOUND TDC id:%x", tdc_2->GetID()) << endl;

  // cout << Form("hook counter: %d", hook_counter) << endl;

  // //  TH1* hist = (TH1*) tdc_1->GetChannelRefHist(chId); // argument is the channel number
  // TH1* tot_1 = (TH1*) tdc_1->GetHist(chId,5); // arguments are the channel number and the ToT histogram number
  // //  TH1* hist = (TH1*) tdc_2->GetChannelRefHist(chId); // argument is the channel number
  // TH1* tot_2 = (TH1*) tdc_2->GetHist(chId,5); // arguments are the channel number and the ToT histogram number

//   if (hook_counter==0) 
//     {
//       printf("Set pulser\n");

//       gSystem->Exec(Form("perl ~/git_cahitugur/scripts/telnet_command_send.pl -pt 0x%x -pr 0x1fff -st 1", ((2**width_counter)-1)));
//       //cout << Form("perl ~/git_cahitugur/scripts/telnet_command_send.pl -pt 0x%x -pr 0x1fff -st 1", ((2**width_counter)-1)) << endl;
//     }
//   else if(hook_counter == 2)
//     {
//       cout << Form("Clear histograms\n") << endl;
//       //      tdc_1->ClearChannelRefHist(chId);
//       tdc_1->ClearHist(chId, 5);
//       //      tdc_2->ClearChannelRefHist(chId);
//       tdc_2->ClearHist(chId, 5);
//     }

//   else if(hook_counter == 10)
//     {
//       cout << Form("pulseWidth:%5.3f TDC:%x mean:%5.3f rms:%5.3f TDC:%x mean:%5.3f rms:%5.3f", width_counter*1.667, tdc_1->GetID(), tot_1->GetMean(), tot_1->GetRMS(), tdc_2->GetID(), tot_2->GetMean(), tot_2->GetRMS()) << endl;
//       gSystem->Exec(Form("echo %5.3f %x %5.3f %5.3f %x %5.3f %5.3f >> ~/git_cahitugur/trb3/measurements/ToT/pulseWidth_scan.txt", width_counter*1.667, tdc_1->GetID(), tot_1->GetMean(), tot_1->GetRMS(), tdc_2->GetID(), tot_2->GetMean(), tot_2->GetRMS()));

//       width_counter++;
//       hook_counter = 10;
//       continue;
//     }
//   hook_counter++;
// }
