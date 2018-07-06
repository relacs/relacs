/*
  efield/dualbeat.h
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

#ifndef _RELACS_EFIELD_DUALBEAT_H_
#define _RELACS_EFIELD_DUALBEAT_H_ 1
/*!
\class DualBeat
\brief [RePro] Output of two beats for discrimination tasks
\author Jan Grewe
\version 1.0 (Jun 12, 2014)
*/

#include <QLabel>
#include <QString>
#include <QPushButton>
#include <relacs/plot.h>
#include <relacs/optwidget.h>
#include <relacs/repro.h>
#include <relacs/efield/traces.h>
#include <relacs/efield/eodtools.h>

using namespace relacs;

namespace efield {

class OutputSetup 
{
  public:
    int channel;
    double freq;
    double ampl;
    bool rewarded;
    bool is_test;
    bool is_harmonic;
    OutputSetup();
    OutputSetup(int channel, double frequency, double amplitude, bool is_rewarded, bool is_test);
};

class TrialResult
{
 public:
  int rewarded, chosen;
  double fish_freq, freq1, freq2;
  double amp1, amp2;
  bool is_test, was_saved;
  double start_time, choice_time;
  TrialResult();
};

class DualBeat : public RePro, public Traces, public EODTools
{
  Q_OBJECT

public:

  DualBeat( void );
  virtual int main( void );

 private:
  OutputSetup channels[2];
  TrialResult result;
  OptWidget JW;
  QPushButton *startButton;
  QPushButton *restartButton;
  QPushButton *nextButton;
  QPushButton *resetButton;
  QPushButton *choice1Button;
  QPushButton *choice2Button;
  QPushButton *noChoiceButton;
  QPushButton *testButton;

  QLabel *Reward1Label;
  QLabel *Reward2Label;
  QLabel *EODLabel;
  QLabel *Out1FreqLabel;
  QLabel *Out2FreqLabel;
  QLabel *Out1AmplLabel;
  QLabel *Out2AmplLabel;
  QLabel *TrialNoLabel;
  QLabel *DurationLabel;
  QLabel *ConditionLabel;
  QLabel *ActiveLabel;

  bool start;
  bool no_fish;
  bool randomize_amplitude;
  bool is_test;
  bool new_trial;
  bool harmonics;
  size_t count;
  int trials, test_trials;
  double fishfreq = 0.0;
  std::vector<int> trial_condition;
  std::vector<int> reward_position;
  std::vector<int> choices;
  std::ofstream file;
  Options metadata;
  TableKey key;
  OutList out_list;
  OutData signal_1;
  OutData signal_2;
  
  void preConfig( void);
  void createLayout( void );
  void populateOptions( void );
  void createConditionOrder( int training_trials, int test_trials, std::vector<int> &conditions);
  void createRewardOrder( int trials, std::vector<int> &reward_position);
  void setupChannels( void );
  void createStimuli( void );
  double estimateEOD( void );
  void createOutput( void );
  void setInfo( void );
  void saveResult( void );
  void getSplus( double &splus_df, double &splus_ampl, bool &splus_harmonic );
  void getSminus( double &sminus_df, double &sminus_ampl, bool &sminus_harmonic );
  void createHarmonicStimulus( OutData &signal, double duration, double stepsize, double frequency, double amplitude, double scale);

  public slots:
    void startClicked( void );
    void testClicked( void);
    void nextClicked( void );
    void resetClicked( void );
    void choice1Clicked( void );
    void choice2Clicked( void );
    void noChoiceClicked( void );
    void restartClicked( void );
 protected:
  virtual void keyPressEvent( QKeyEvent *e );
  virtual void customEvent( QEvent *qce );

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_DUALBEAT_H_ */
