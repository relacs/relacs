/*
  efield/dualbeat.cc
  Output of two beats for discrimination tasks

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  RELACS is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <relacs/map.h>
#include <relacs/eventdata.h>
#include <relacs/tablekey.h>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <math.h>
#include <algorithm>
//#include <relacs/base/linearattenuate.h>
#include <relacs/efield/dualbeat.h>
using namespace relacs;

namespace efield {
  /**
     OutputSetup
  **/
  OutputSetup::OutputSetup(): channel(-1), freq(0), ampl(0), rewarded(false), is_test(false){}
  OutputSetup::OutputSetup(int channel, double frequency, double amplitude, bool is_rewarded, bool is_test): channel(channel), freq(frequency), ampl(amplitude), rewarded(is_rewarded), is_test(is_test){}


  /**
     DualBeatEvent
  **/
  class DualBeatEvent : public QEvent
  {

  public:

    DualBeatEvent( double val )
      : QEvent( Type( User+14 ) ),
        Value( val )
    {
    }

    double Value;
  };

  /**
     TrialResult constructor
  **/
  TrialResult::TrialResult():rewarded(-1), chosen(-1), fish_freq(0.0), freq1(0.0), freq2(0.0), amp1(0.0), amp2(0.0), is_test(false), was_saved(false), start_time(0.0), choice_time(0.0){}

  /**
     DualBeat
  **/
  DualBeat::DualBeat( void )
    : RePro( "DualBeat", "efield", "Jan Grewe", "1.0", "Jun 12, 2014" )
  {
    this->populateOptions();
    this->createLayout();
    srand( std::time( NULL ) );
    count = 0;
  }

  void DualBeat::preConfig( void )
  {
    setText( "intrace", traceNames() );
  }


  void DualBeat::createLayout( void ) {
    QGridLayout *grid = new QGridLayout;
    setLayout( grid );
    JW.assign( (Options*)this, 2, 4, true, 0, mutex() );

    QGroupBox *infoBox = new QGroupBox( "Info" );
    QGridLayout *infoLayout = new QGridLayout;
    QLabel *TrialHeader = new QLabel( "Trial:" );
    infoLayout->addWidget( TrialHeader, 0, 0, 1, 1);
    TrialNoLabel = new QLabel( "---" );
    infoLayout->addWidget( TrialNoLabel, 0, 1, 1, 1);
    QLabel *EODHeader = new QLabel( "Fish EODf [Hz]:" );
    infoLayout->addWidget( EODHeader, 1, 0, 1, 1 );
    EODLabel = new QLabel( "---" );
    infoLayout->addWidget( EODLabel, 1, 1, 1, 1 );
    QLabel *Out1FreqHeader = new QLabel( "Electrode 1 freq [Hz]:" );
    infoLayout->addWidget( Out1FreqHeader, 2, 0, 1, 1 );
    Out1FreqLabel = new QLabel( "---" );
    infoLayout->addWidget( Out1FreqLabel, 2, 1, 1, 1 );
    QLabel *Out1AmplHeader = new QLabel( "Electrode 1 ampl [V]:" );
    infoLayout->addWidget( Out1AmplHeader, 3, 0, 1, 1 );
    Out1AmplLabel = new QLabel( "---" );
    infoLayout->addWidget( Out1AmplLabel, 3, 1, 1, 1 );

    QLabel *Out2FreqHeader = new QLabel( "Electrode 2 freq [Hz]:" );
    infoLayout->addWidget( Out2FreqHeader, 4, 0, 1, 1 );
    Out2FreqLabel = new QLabel( "---" );
    infoLayout->addWidget( Out2FreqLabel, 4, 1, 1, 1 );

    QLabel *Out2AmplHeader = new QLabel( "Electrode 2 ampl [V]:" );
    infoLayout->addWidget( Out2AmplHeader, 5, 0, 1, 1 );
    Out2AmplLabel = new QLabel( "---" );
    infoLayout->addWidget( Out2AmplLabel, 5, 1, 1, 1 );

    QLabel *DurationHeader = new QLabel( "Trial duration [s]:" );
    infoLayout->addWidget( DurationHeader, 6, 0, 1, 1 );
    DurationLabel = new QLabel( "---" );
    infoLayout->addWidget( DurationLabel, 6, 1, 1, 1 );

    QLabel *ConditionHeader = new QLabel( "Trial condition:" );
    infoLayout->addWidget( ConditionHeader, 7, 0, 1, 1 );
    ConditionLabel = new QLabel( "---" );
    infoLayout->addWidget( ConditionLabel, 7, 1, 1, 1 );

    ActiveLabel = new QLabel( "Idle" );
    ActiveLabel->setAlignment(Qt::AlignCenter);
    infoLayout->addWidget( ActiveLabel, 8, 0, 1, 2 );
    infoBox->setLayout( infoLayout );

    Reward1Label = new QLabel( "Electrode 1" );
    Reward1Label->setAlignment( Qt::AlignCenter );
    Reward1Label->setStyleSheet( "background-color:red" );
    Reward2Label = new QLabel( "Electrode 2" );
    Reward2Label->setAlignment( Qt::AlignCenter );
    Reward2Label->setStyleSheet( "background-color:green" );

    choice1Button = new QPushButton( "Electrode 1" );
    choice1Button->setMinimumHeight( 40 );
    choice1Button->setEnabled( false );
    connect( choice1Button, SIGNAL( clicked() ), this, SLOT( choice1Clicked() ) );

    noChoiceButton = new QPushButton( "No choice" );
    noChoiceButton->setMinimumHeight( 40 );
    noChoiceButton->setEnabled( false );
    connect( noChoiceButton, SIGNAL( clicked() ), this, SLOT(  noChoiceClicked() ) );

    choice2Button = new QPushButton( "Electrode 2" );
    choice2Button->setMinimumHeight( 40 );
    choice2Button->setEnabled( false );
    connect( choice2Button, SIGNAL( clicked() ), this, SLOT(  choice2Clicked() ) );

    QGroupBox *rewardBox = new QGroupBox( "Rewards" );
    QGroupBox *choiceBox = new QGroupBox( "Choice" );

    QVBoxLayout *cb = new QVBoxLayout;
    cb->addWidget( choice1Button );
    cb->addWidget( noChoiceButton );
    cb->addWidget( choice2Button );
    choiceBox->setLayout( cb );

    QVBoxLayout *rb = new QVBoxLayout;
    rb->addWidget( Reward1Label );
    rb->addWidget( Reward2Label );
    rewardBox->setLayout( rb );

    QGroupBox *btnBox = new QGroupBox;
    QHBoxLayout *btnLyt = new QHBoxLayout;

    startButton = new QPushButton( "&start experiment" );
    connect( startButton, SIGNAL( clicked() ), this, SLOT( startClicked() ) );
    grabKey( Qt::ALT+Qt::Key_S );

    restartButton = new QPushButton( "Restart trial" );
    restartButton->setEnabled( false );
    connect( restartButton, SIGNAL( clicked() ), this, SLOT( restartClicked() ) );

    nextButton = new QPushButton( "Next trial" );
    nextButton->setEnabled( false );
    connect( nextButton, SIGNAL( clicked() ), this, SLOT( nextClicked() ) );

    resetButton = new QPushButton( "Reset" );
    connect( resetButton, SIGNAL( clicked() ), this, SLOT( resetClicked() ) );

    testButton = new QPushButton( "Test trial" );
    connect( testButton, SIGNAL( clicked() ), this, SLOT( testClicked() ) );

    btnLyt->addWidget( startButton );
    btnLyt->addWidget( nextButton );
    btnLyt->addWidget( restartButton );
    btnLyt->addWidget( resetButton );
    btnLyt->addWidget( testButton );
    btnBox->setLayout( btnLyt );

    grid->addWidget( infoBox, 0, 0, 3, 1 );
    grid->addWidget( rewardBox, 0, 1, 3, 1 );
    grid->addWidget( choiceBox, 0, 2, 3, 1 );
    grid->addWidget( btnBox, 3, 0, 1, 3 );
  }


  void DualBeat::populateOptions( void ) {
    newSection( "Stimulus" );
    addSelection( "rewarded", "rewarded stimulus", "A|B" );
    addNumber( "eodf", "Current EOD frequency", 0.0, 0.0, 10000.0, 1.0, "Hz", "Hz", "%.1f" );
    addNumber( "duration", "Signal duration", 10.0, 0.0, 1000000.0, 1.0, "seconds" );
    addNumber( "deltafA", "Difference frequency A", 25.0, -2000.0, 2000.0, 1.0, "Hz", "Hz", "%.1f" );
    addBoolean( "harmonicA", "Create signal A with 1st harmonic.", false);
    addNumber( "amplitudeA", "Amplitude  A", 1.0, 0.0, 10.0, 1.0, "V", "V", "%.3f" );
    addNumber( "deltafB", "Difference frequency B", -50.0, -2000.0, 2000.0, 1.0, "Hz", "Hz", "%.1f" );
    addBoolean( "harmonicB", "Create signal B with 1st harmonic.", false);
    addNumber( "amplitudeB", "Amplitude B", 1.0, 0.0, 10.0, 1.0, "V", "V", "%.3f" );

    newSection( "Experiment" );
    addBoolean( "noFish", "Test mode without fish", false );
    addBoolean( "randomizeAmpl", "Randomize amplitude", true );
    addNumber( "amplitudeRange", "Randomization range", 20.0, 0.0, 50.0, 1.0, "%" ).setActivation( "randomAmpl", "true" );
    addBoolean( "randomSminus", "Randomize s-", false );
    addNumber( "sminusRange", "Randomization range", 200.0, 0.0, 2000.0, 1.0, "Hz" ).setActivation( "randomSminus", "true" );
    addBoolean( "training", "Is training experiment", true );
    addBoolean( "randomElectrode", "Randomize the output side", true );
    addSelection( "rewardedElectrode", "rewarded electrode", "1|2" ).setActivation("randomElectrode", "false" );
    addNumber( "trainingTrials", "Number of training trials", 10.0, 0.0, 100.0 );
    addNumber( "testTrials", "Number of test trials", 5.0, 0.0, 100.0 );
    addNumber( "testBeat", "Difference frequency of tests",  -25.0, -2000.0, 2000.0, 1.0, "Hz", "Hz", "%.1f" );
    addNumber( "testAmplitude", "Test amplitude", 1.0, 0.0, 10.0, 1.0, "V", "V", "%.3f" );
    addBoolean( "stimOff", "Switch stimulus off upon decision", true );
    addBoolean( "rewardInTest", "Also reward in test trials if S+ present", true );
    addBoolean( "harmonic", "Create test stim via first harmonic", true );

    newSection( "Setup" );
    addNumber( "scaling1", "Scaling Electrode 1", 1.0, 0.0, 10.0, 0.01, "", "", "%.3f" );
    addNumber( "scaling2", "Scaling electrode 2", 1.0, 0.0, 10.0, 0.01, "", "", "%.3f" );

    newSection( "EOD estimation" );
    addSelection( "intrace", "inputTrace" );
    addBoolean( "usepsd", "Use the power spectrum", true );
    addNumber( "mineodfreq", "Minimum expected EOD frequency", 100.0, 0.0, 10000.0, 10.0, "Hz" ).setActivation( "usepsd", "true" );
    addNumber( "maxeodfreq", "Maximum expected EOD frequency", 2000.0, 0.0, 10000.0, 10.0, "Hz" ).setActivation( "usepsd", "true" );
    addNumber( "eodfreqprec", "Precision of EOD frequency measurement", 1.0, 0.0, 100.0, 0.1, "Hz" ).setActivation( "usepsd", "true" );
    addNumber( "averagetime", "Time for computing EOD frequency", 2.0, 0.0, 100000.0, 1.0, "s" );

    // File header
    metadata.newSection( "Stimulus" );
    metadata.addText( "rewarded" );
    metadata.addNumber( "EOD frequency", "Hz", "%.1f" );
    metadata.addNumber( "Signal duration", "seconds", "%.2f" );
    metadata.addNumber( "Df A", "Hz", "%.1f" );
    metadata.addText( "harmonic A" );

    metadata.addNumber( "Amplitude A", "V", "%.3f" );
    metadata.addNumber( "Df B", "Hz", "%.1f" );
    metadata.addText( "harmonic B" );
    metadata.addNumber( "Amplitude B", "V", "%.3f" );
    metadata.addNumber( "Test beat", "Hz", "%.1f" );
    metadata.addNumber( "Test amplitude", "V", "%.3f" );
    metadata.addText( "S- randomization" );
    metadata.addNumber( "S- range", "Hz", "%0.1f" );

    metadata.newSection( "Experiment" );
    metadata.addText( "Test without fish" );
    metadata.addText( "Randomize amplitude" );
    metadata.addNumber( "Randomization range", "%", "%.1f" );
    metadata.addText( "Is training" );
    metadata.addText( "Random electrode" );
    metadata.addNumber( "Rewarded electrode", "%", "%.1f" );
    metadata.addNumber( "Number of training trials" );
    metadata.addNumber( "Number of test trials" );
    metadata.addText( "Test stim via harmonic" );
    metadata.addText( "Reward in test");

    metadata.newSection( "Setup" );
    metadata.addNumber( "Scaling Electrode 1", "", "%.3f" );
    metadata.addNumber( "Scaling Electrode 2", "", "%.3f" );

    // table key
    key.addNumber( "rewarded", "", "%.0f" );
    key.addNumber( "chosen", "", "%.0f" );
    key.addNumber( "is_test", "", "%.0f" );
    key.addNumber( "amplitude1", "V", "%.3f" );
    key.addNumber( "amplitude2", "V", "%.3f" );
    key.addNumber( "eodf", "Hz", "%.2f" );
    key.addNumber( "freq1", "Hz", "%.2f" );
    key.addNumber( "freq2", "Hz", "%.2f" );
    key.addNumber( "start_time", "s", "%.2f" );
    key.addNumber( "choice_time", "s", "%.2f" );
  }


  void DualBeat::createRewardOrder( int trials, std::vector<int> &reward_position )
  {
    reward_position.clear();
    if ( !boolean( "randomElectrode" ) ) {
      int rewardedElectrode = number( "rewardedElectrode" );
      for (int i = 0; i < trials; i++) {
        reward_position.push_back( rewardedElectrode-1 );
      }
    }
    else {
      for (int i = 0; i < trials/2; i++) {
        reward_position.push_back(0);
      }
      for (int i = 0; i < (trials - trials/2); i++) {
        reward_position.push_back(1);
      }
      std::random_shuffle( reward_position.begin(), reward_position.end() );
    }
  }


  void DualBeat::createConditionOrder( int training_trials, int test_trials, std::vector<int> &conditions )
  {
    conditions.clear();
    conditions.reserve(training_trials + test_trials);
    for (int i = 0; i < training_trials; i++) {
      conditions.push_back(0);
    }
    for (int i = 0; i < test_trials; i++) {
      conditions.push_back(1);
    }
    std::random_shuffle( conditions.begin(), conditions.end() );
    // debug output
    std::cerr << "condition sequence:\n\t ";
    for (size_t i =0; i < conditions.size(); i++) {
      std::cerr << conditions[i] << ", ";
    }
    std::cerr << std::endl;
  }


  void DualBeat::setInfo() {
    EODLabel->setText( Str(fishfreq, "%4.0f").c_str() );
    string trial_text = Str(count+1, "%i") + " of " + Str( trial_condition.size() );
    TrialNoLabel->setText( trial_text.c_str() );
    Out1FreqLabel->setText( Str(channels[0].freq, "%4.0f").c_str() );
    Out2FreqLabel->setText( Str(channels[1].freq, "%4.0f").c_str() );
    Out1AmplLabel->setText( Str(channels[0].ampl, "%4.3f").c_str() );
    Out2AmplLabel->setText( Str(channels[1].ampl, "%4.3f").c_str() );
    DurationLabel->setText( Str(number( "duration" ), "%4.2f").c_str() );
    string condition = trial_condition[count] == 0 ? "Training":"Test";
    ConditionLabel->setText( condition.c_str() );
  }


  void DualBeat::getSminus(double &sminus_df, double &sminus_ampl, bool &sminus_harmonic ) {
    double splus_df = 0.0;
    if ( text( "rewarded" ) == "A" ){
      splus_df = number( "deltafA" );
      sminus_df = number( "deltafB" );
      sminus_ampl = number( "amplitudeB" );
      sminus_harmonic = boolean( "harmonicB" );
    } else {
      splus_df = number( "deltafB" );
      sminus_df = number( "deltafA" );
      sminus_ampl = number( "amplitudeA" );
      sminus_harmonic = boolean( "harmonicA" );
    }
    if ( boolean( "randomSminus" ) ){
      double sMinus_range = number( "sminusRange" );
      int sign = (rand() % 2) == 1 ? -1:1;
      int df = (rand() % 10) * sMinus_range / 10;
      while ( df == 0 || df == splus_df ){
        df = (rand() % 10) * sMinus_range / 10;
      }
      sminus_df = sign * df;
    }
  }


  void DualBeat::getSplus(double &splus_df, double &splus_ampl, bool &splus_harmonic) {
    if ( text( "rewarded" ) == "A" ){
      splus_df = number( "deltafA" );
      splus_ampl = number( "amplitudeA" );
      splus_harmonic = boolean( "harmonicA" );
    } else {
      splus_df = number( "deltafB" );
      splus_ampl = number( "amplitudeB" );
      splus_harmonic = boolean( "harmonicB" );
    }
  }
  

  void DualBeat::setupChannels() {
    string rewarded = text( "rewarded" );
    int amplRange = number( "amplitudeRange" );
    double test_beat = number( "testBeat" );
    double test_amplitude = number( "testAmplitude" );
    double sminus_df, splus_df;
    double sminus_ampl, splus_ampl;
    bool splus_harmonic, sminus_harmonic;
    getSminus(sminus_df, sminus_ampl, sminus_harmonic);
    getSplus(splus_df, splus_ampl, splus_harmonic);
    randomize_amplitude = boolean( "randomizeAmpl" );
    harmonics = boolean( "harmonic" );
   
    if ( is_test ) { //play-around test trial...
      channels[0].freq = fishfreq + splus_df;
      channels[1].freq = fishfreq + sminus_df;
      channels[0].is_harmonic = splus_harmonic;
      channels[1].is_harmonic = sminus_harmonic;
      channels[0].is_test = true;
      channels[1].is_test = true;
      channels[0].ampl = splus_ampl;
      channels[1].ampl = sminus_ampl;
      if ( rewarded == "A" ) {
	channels[0].rewarded = true;
	channels[1].rewarded = false;
      } else {
	channels[0].rewarded = false;
	channels[1].rewarded = true;
      }
    }
    else {
      if ( trial_condition[count] == 0 ) { //is training
	channels[0].is_test = false;
	channels[1].is_test = false;
        channels[ reward_position[count] ].rewarded = true;
        channels[ 1 - reward_position[count] ].rewarded = false;
        channels[ reward_position[count] ].freq = fishfreq + splus_df;
        channels[ 1 - reward_position[count] ].freq = fishfreq + sminus_df;
        channels[ reward_position[count] ].is_harmonic = splus_harmonic;
        channels[ 1 - reward_position[count] ].is_harmonic = sminus_harmonic;
        channels[ reward_position[count] ].ampl = splus_ampl;
        channels[ 1 - reward_position[count] ].ampl = sminus_ampl;
      } else { //test trial - no reward -
	channels[0].is_test = true;
	channels[1].is_test = true;
        channels[ reward_position[count] ].rewarded = true;
        channels[ 1 - reward_position[count] ].rewarded = false;
        channels[ reward_position[count] ].freq = fishfreq + splus_df;
        channels[ 1 - reward_position[count] ].freq = fishfreq + test_beat;
        channels[ reward_position[count] ].is_harmonic = splus_harmonic;
        channels[ 1 - reward_position[count] ].is_harmonic = harmonics;
        channels[ reward_position[count] ].ampl = splus_ampl;
        channels[ 1 - reward_position[count] ].ampl = test_amplitude;
      }
    }
    double rndScale_1 = 1.0;
    double rndScale_2 = 1.0;
    if ( randomize_amplitude ) {
      int plusminus = rand() % 2;
      int scale = rand() % amplRange;
      if ( plusminus == 0 ) { //electrode 1 stronger than 2
	rndScale_1 += scale/100.;
      }
      else {
	rndScale_2 += scale/100.;
      }
    }
    channels[0].ampl *= rndScale_1;
    channels[1].ampl *= rndScale_2;
  }


  void DualBeat::createHarmonicStimulus( OutData &signal , double duration, double stepsize, double frequency, double amplitude, double scale) {
    double f_1_phase = 0.5;
    double f_0 = frequency / 2.;
    double f_0_ampl = amplitude * 2.;
    vector<double> stim;
    for ( double t = 0.0; t < duration; t += stepsize ) {
      stim.push_back( ( (f_0_ampl * sin(t * f_0 * 2 * M_PI ) ) + ( amplitude * sin( t * frequency * 2 * M_PI + f_1_phase) ) ) * scale  / 2.);
    }
    signal.assign( stim.data(), stim.size(), stepsize );
  }


  void DualBeat::createStimuli()
  {
    double duration = number( "duration" );
    string rewarded = text( "rewarded" );
    double scale_1 = number( "scaling1");
    double scale_2 = number( "scaling2" );
    double sminus_df, splus_df;
    double sminus_ampl, splus_ampl;
    bool sminus_harmonic, splus_harmonic;
    getSminus(sminus_df, sminus_ampl, sminus_harmonic);
    getSplus(splus_df, splus_ampl, splus_harmonic);
    randomize_amplitude = boolean( "randomizeAmpl" );
    harmonics = boolean( "harmonic" );
    double stepsize = 0.00002;
    
    fishfreq = estimateEOD();
    setNumber( "eodf", fishfreq );
    setupChannels();
    setInfo();
    result = TrialResult();
    result.start_time = currentTime();
    result.rewarded = (channels[0].rewarded ? 1 : 2);
    result.is_test = channels[0].is_test;
    result.amp1 = channels[0].ampl;
    result.amp2 = channels[1].ampl;
    result.fish_freq = fishfreq;
    result.freq1 = channels[0].freq;
    result.freq2 = channels[1].freq;

    postCustomEvent( 16 );
    signal_1.clear();
    signal_1.setTrace( LocalEField[0] );
    if ( channels[0].is_harmonic ) {
      createHarmonicStimulus(signal_1, duration, stepsize, channels[0].freq, channels[0].ampl, scale_1 );
    } else {
      signal_1.sineWave( duration, stepsize, channels[0].freq, 0.0, (channels[0].ampl * scale_1)/2. );
    }
    signal_1.setIdent( "sinewave" );

    signal_2.clear();
    signal_2.setTrace( LocalEField[1] );
    if ( channels[1].is_harmonic ) {
      createHarmonicStimulus(signal_2, duration, stepsize, channels[1].freq, channels[1].ampl, scale_2 );
    } else {
      signal_2.sineWave( duration, stepsize, channels[1].freq, 0.0, (channels[1].ampl * scale_2)/2. );
    }
    signal_2.setIdent( "sinewave" );

    out_list.clear();
    out_list.push( signal_1 );
    out_list.push( signal_2 );
  }


  void DualBeat::createOutput() {
    metadata.setText( "rewarded", text( "rewarded" ) );
    metadata.setNumber( "EOD frequency", number( "eodf" ) );
    metadata.setNumber( "Signal duration", number ( "duration" ) );
    metadata.setNumber( "Df A", number( "deltafA" ) );
    metadata.setText( "harmonic A", text( "harmonicA" ) );
    metadata.setNumber( "Df B", number( "deltafB" ) );
    metadata.setText( "harmonic B", text( "harmonicB" ) );
    metadata.setNumber( "Amplitude A", number( "amplitudeA" ) );
    metadata.setNumber( "Amplitude B", number( "amplitudeB" ) );
    metadata.setNumber( "Test beat", number( "testBeat" ) );
    metadata.setNumber( "Test amplitude", number( "testAmplitude" ) );

    metadata.setText( "Test without fish", (boolean("noFish") ? "True":"False" ) );
    metadata.setText( "Randomize amplitude", (boolean("randomizeAmpl") ? "True":"False" ) );
    metadata.setNumber( "Randomization range", number( "amplitudeRange" ) );
    metadata.setText( "S- randomization", (boolean("randomizeAmpl") ? "True":"False" ) );
    metadata.setNumber( "S- range", number( "sminusRange" ) );
    metadata.setText( "Is training", (boolean("isTest") ? "False":"True" ) );
    metadata.setText( "Random electrode", (boolean("randomElectrode") ? "False":"True" ) );
    metadata.setNumber( "Rewarded electrode", number( "rewardedElectrode" ) );
    metadata.setNumber( "Number of training trials", number( "trainingTrials" ) );
    metadata.setNumber( "Number of test trials", number( "testTrials" ) );
    metadata.setText( "Reward in test", (boolean("rewardInTest") ? "False":"True" ) );
    metadata.setText( "Test stim via harmonic", (boolean("harmonic") ? "False":"True" ) );

    metadata.setNumber( "Scaling Electrode 1", number( "scaling1" ) );
    metadata.setNumber( "Scaling Electrode 2", number( "scaling2" ) );
    string filename("choices.dat");

    if ( !sessionRunning() ){
      cerr << "Session is not running! Choices wont be saved!" << endl;
      return;
    }
    file.open( addPath(filename).c_str(), ofstream::out | ofstream::app );

    if ( file.good() ) {
      file << "\n\n";
      metadata.save( file, "# " );
      file << "\n";
      key.saveKey( file, true, false );
    }
  }


  double DualBeat::estimateEOD( void ) {
    double averagetime = number( "averagetime" );
    if ( boolean( "no_fish" ) ) {
      return 0.0;
    } else {
      if ( !boolean( "usepsd" ) ) {
	return events( EODEvents ).frequency( currentTime()-averagetime, currentTime() ); 
	if ( EODEvents < 0 ) {
	  warning( "need EOD events of the EOD Trace." );
	  return 0.0;
	}
      } else {
	double bigeod = 0.0;
	double bigeodf = 0.0; 
	double min_eodf = number( "mineodfreq" );
	double max_eodf = number( "maxeodfreq" );
	double eodf_prec = number( "eodfreqprec" );
	double fishrate;
	int intrace = index( "intrace" );
	int nfft = 1;
	
	nfft = nextPowerOfTwo( (int)::ceil( 1.0/trace( intrace ).stepsize()/eodf_prec ) );
	eodf_prec = 1.0/trace( intrace ).stepsize()/nfft;
	if ( averagetime < 2.0/trace( intrace ).stepsize()/nfft ) {
	  averagetime = 2.0/trace( intrace ).stepsize()/nfft;
	  warning( "averagetime is too small for requested frequency resolution. Set it to " +
		   Str( averagetime ) + "s for now." );
	}
	
	SampleDataF data( 0.0, averagetime, trace( intrace ).stepsize() );
	trace( intrace ).copy( currentTime() - averagetime, data );
	SampleDataF power( nfft );
	rPSD( data, power );
	double threshold = power.max( min_eodf, max_eodf );
	EventData powerpeaks( 1000, true );
	peaks( power, powerpeaks, 0.2*threshold );
	double maxpower = 0.0;
	double maxfreq = 0.0;
	for ( int i=0; i<powerpeaks.size(); i++ ) {
	  if ( powerpeaks[i] >= min_eodf && powerpeaks[i] <= max_eodf ) {
	    if ( powerpeaks.eventSize( i ) > maxpower ) {
	      maxpower = powerpeaks.eventSize( i );
	      maxfreq = powerpeaks[i];
	    }
	  }
	}
	if ( bigeod < maxpower ) {
	  bigeod = maxpower;
	  bigeodf = maxfreq;
	}
	fishrate = bigeodf;
	return fishrate;
      }
    }
    return 0.0;
  }


  int DualBeat::main( void )
  {
    double starttime = currentTime();
    double duration = number( "duration" );
    string msg;
    noSaving();
    is_test = false;
    channels[0].channel = 1;
    channels[1].channel = 2;
    
    start = false;
    keepFocus();

    while ( softStop() == 0 ) {
      if ( interrupt() || softStop() > 0 )
        break;
      if ( !start ){
        sleep(0.2);
        continue;
      }
      starttime = currentTime();
      msg = "Freq_1:  <b>" + Str( channels[0].freq, 0, 1, 'f' ) + "Hz</b>";
      msg += " Freq_2:  <b>" + Str( channels[1].freq, 0, 1, 'f' ) + "Hz</b>";
      msg += " EODf: <b>" + Str( number( "eodf" ), "%g") + "Hz</b>";
      message(msg);

      startWrite( out_list );
      if ( signal_1.failed() || signal_2.failed() ) {
        msg = "Output of stimulus failed!<br>Error code is <b>";
        msg += signal_1.errorText() + "</b>";
        warning( msg, 2.0 );
        writeZero( LocalEField[0] );
        writeZero( LocalEField[1] );
        return Failed;
      }

      do {
        QCoreApplication::postEvent( this, new DualBeatEvent( currentTime() - starttime ) );
        sleep( 0.2 );
        if ( interrupt() ) {
          writeZero( LocalEField[0] );
          writeZero( LocalEField[1] );
          return Aborted;
        }
        if ( new_trial ){
          starttime = currentTime();
          new_trial = false;
        }
      } while ( (currentTime() - starttime <  duration) && start );
      start = false;
      postCustomEvent( 15 );
    }
    if(file.good())
      file.close();
    writeZero( LocalEField[0] );
    writeZero( LocalEField[1] );
    return Completed;
  }


  void DualBeat::choice1Clicked( void )
  {
    result.chosen = 1;
    result.choice_time = currentTime();
    saveResult();
    postCustomEvent( 19 );
    postCustomEvent( 17 );
    if ( boolean( "stimOff" ) ) {
      start = false;
      stopWrite();
    }
  }


  void DualBeat::choice2Clicked( void )
  {
    result.chosen = 2;
    result.choice_time = currentTime();
    saveResult();
    postCustomEvent( 19 );
    postCustomEvent( 17 );
    if ( boolean( "stimOff" ) ) {
      start = false;
      stopWrite();
    }
  }


  void DualBeat::noChoiceClicked( void )
  {
    result.chosen = 0;
    result.choice_time = currentTime();
    saveResult();
    postCustomEvent( 19 );
    postCustomEvent( 17 );
    if ( boolean( "stimOff" ) ) {
      start = false;
      stopWrite();
    }
  }


  void DualBeat::testClicked( void )
  {
    start = true;
    is_test = true;
    createStimuli();
    ActiveLabel->setText( "Output active" );
    start = true;
    is_test = true;
    JW.accept();
    wake();
  }


  void DualBeat::nextClicked( void )
  {
    start = false;
    stopWrite();
    count ++;
    createStimuli();
    postCustomEvent( 18 );
    JW.accept();
    start = true;
    is_test = false;
    new_trial = true;
    wake();
  }


  void DualBeat::resetClicked( void )
  {
    stopWrite( );
    start = false;
    is_test = false;
    postCustomEvent( 21 );
    count = -1;

    TrialNoLabel->setText( "---" );
    EODLabel->setText( "---" );
    Out1FreqLabel->setText( "---" );
    Out1AmplLabel->setText( "---" );
    Out2FreqLabel->setText( "---" );
    Out2AmplLabel->setText( "---" );
    DurationLabel->setText( "---" );
    ActiveLabel->setText( "Idle" );
  }


  void DualBeat::restartClicked( void )
  {
    start = false;
    stopWrite();
    createStimuli();
    postCustomEvent( 18 );
    JW.accept();
    start = true;
    is_test = false;
    new_trial = true;
    wake();
  }


  void DualBeat::startClicked( void )
  {
    int test_trials = 0;
    if ( !boolean( "training" ) ){
      test_trials = number( "testTrials" );
    }
    int train_trials = number( "trainingTrials" );
    createConditionOrder( train_trials, test_trials, trial_condition );
    createRewardOrder( train_trials + test_trials, reward_position );
    choices.clear();
    count = -1;
    startButton->setEnabled( false );
    createOutput();
    nextClicked();
  }


  void DualBeat::customEvent( QEvent *qce )
  {
    switch ( qce->type() - QEvent::User ) {
    case 11: {
      startButton->setEnabled( true );
      if ( JW.firstWidget() != 0 )
        JW.firstWidget()->setFocus( Qt::TabFocusReason );
      break;
    }
    case 12: {
      removeFocus();
      startButton->setEnabled( false );
      break;
    }
    case 14: {
      DualBeatEvent *dbe = dynamic_cast<DualBeatEvent*>( qce );
      int secs = (int)::round( dbe->Value );
      int mins = secs / 60;
      secs -= mins * 60;
      int hours = mins / 60;
      mins -= hours * 60;
      string ts = "";
      if ( hours > 0 )
        ts += Str( hours ) + ":";
      ts += Str( mins, 2, '0' ) + ":";
      ts += Str( secs, 2, '0' );
      // ElapsedTimeLabel->setText( ts.c_str() );
      break;
    }
    case 15: {
      ActiveLabel->setText( "Idle" );
      break;
    }
    case 16: { //set the reward colors
      if (channels[0].rewarded && (!channels[0].is_test || boolean( "rewardInTest" ))) {
        Reward1Label->setStyleSheet( "background-color:green" );
      } else {
        Reward1Label->setStyleSheet( "background-color:red" );
      }
      if (channels[1].rewarded && (!channels[1].is_test || boolean( "rewardInTest" ))) {
        Reward2Label->setStyleSheet( "background-color:green" );
      } else {
        Reward2Label->setStyleSheet( "background-color:red" );
      }
      break;
    }
    case 17:{ //decide upon activation of next Button
      if ( count >= trial_condition.size()-1 ) {
	nextButton->setEnabled( false );
	if (file.good())
	  file.close();
      }
      else {
	nextButton->setEnabled( true );
      }
      break;
    }
    case(18):{
      choice1Button->setEnabled( true );
      noChoiceButton->setEnabled( true );
      choice2Button->setEnabled( true );
      restartButton->setEnabled( true );
      nextButton->setEnabled( false );
      ActiveLabel->setText( "output active" );
      setInfo();
      break;
    }
    case( 19 ): { // choice done
      choice1Button->setEnabled( false );
      noChoiceButton->setEnabled( false );
      choice2Button->setEnabled( false );
      nextButton->setEnabled( true );
      restartButton->setEnabled( false );
      if ( boolean( "stimOff" ) ) {
        ActiveLabel->setText( "idle" );
      }
      break;
    }
    case ( 20 ): {
      break;
    }
    case (21):{
      choice1Button->setEnabled( false );
      noChoiceButton->setEnabled( false );
      choice2Button->setEnabled( false );
      startButton->setEnabled( true );
      nextButton->setEnabled( false );
      restartButton->setEnabled( false );
      break;
    }
    default:
      RePro::customEvent( qce );
    }
  }


  void DualBeat::saveResult() {
    if ( file.good() && !result.was_saved ) {
      key.save( file, result.rewarded, 0 );
      key.save( file, result.chosen );
      key.save( file, (result.is_test ? 1 : 0 ) );
      key.save( file, result.amp1 );
      key.save( file, result.amp2 );
      key.save( file, result.fish_freq );
      key.save( file, result.freq1 );
      key.save( file, result.freq2 );
      key.save( file, result.start_time );
      key.save( file, result.choice_time );
      file << "\n";
      result.was_saved = true;
    }
    if( !file.good() && sessionRunning() ) {
      cerr << "Error saving data! File no good!" << endl;
    }
  }


  void DualBeat::keyPressEvent( QKeyEvent *e )
  {
    if ( e->key() == Qt::Key_S && ( e->modifiers() & Qt::AltModifier ) ) {
      startButton->animateClick();
      e->accept();
    }
    else
      RePro::keyPressEvent( e );
  }


  addRePro( DualBeat, efield );

}; /* namespace efield */

#include "moc_dualbeat.cc"
