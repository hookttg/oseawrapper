# oseawrapper

Wrapper application to parse a input data file through the Open Source ECG Analyzer by E. P. Limited.

Licensed under the GNU GPL v2.0, see `gpl-2.0.md` for more information.


## Compilation

No known exotic dependencies. Type `make` and you should be done.

Working on _Red Hat Enterprise Linux Workstation release 6.5 (Santiago)_.


## Usage

`./oseawrapper [input datafile] (output datafile)`


### Input datafile

A file containing a single ECG integer sample per line. The data should be formatted to fulfil the following conditions:

* Normalised to 5 mV/LSB (200 A-to-D units per mV)
* 200 Hz sample rate, although this algorithm should work beyond this


### Output datafile

Output is sent to a file or to `stdout` if not specified. Each detected heart beat is reported as a tab-delimited _index_, _delay_, _beat type_ and _beat match_ output.

The _index_ is the sample offset when the beat was detected. The next value, _delay_ is the delay in samples for the OSEA algorithm to detect the heart beat, therefore the beat was detected at `index - delay` samples. The _beat type_ is `1` for a normal R-wave, `5` for a PVC (Premature Ventricular Contraction) and `13` for an unknown beat. The _beat match_ is provided for an unknown debugging purpose.


## Links

http://keot.co.uk/

http://eplimited.com/
