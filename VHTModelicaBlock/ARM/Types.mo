within ARM;
package Types
extends Modelica.Icons.TypesPackage;
type Frequency_kHz = Real (final quantity="Frequency", final unit="kHz") "Frequency in kilo Hertz" annotation(
    Documentation(info = "<html><head></head><body>It is used in some models where the sampling frequency is more naturally expressed in kHz rather than Hz.</body></html>"));
  annotation(
    Documentation(info = "<html><head></head><body>Definition of some types used by the different models of the package.</body></html>"));
end Types;
