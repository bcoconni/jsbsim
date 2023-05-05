#include <cxxtest/TestSuite.h>

#include <FGFDMExec.h>
#include <models/atmosphere/FGMSIS.h>

#include "MSIS_test_data.h"

using namespace JSBSim;

class DummyMSIS : public MSIS
{
public:
  DummyMSIS(FGFDMExec* fdmex) : MSIS(fdmex) {
    in.altitudeASL = 0.0;
    in.GeodLatitudeDeg = 0.0;
    in.LongitudeDeg = 0.0;
  }

  ~DummyMSIS() { PropertyManager->Unbind(this); }

  // Getters for the protected members
  double GetR(void) { return Reng; }
  static constexpr double GetRstar(void) { return Rstar; }
  static constexpr double GetBeta(void) { return Beta; }
  static constexpr double GetSutherlandConstant(void) { return SutherlandConstant; }
  static constexpr double GetPSFtoPa(void) { return psftopa; }
  static constexpr double GetPSFtoInHg(void) { return psftoinhg; }
  // Setters for the protected members
  void SetDay(double day) { day_of_year = day; }
  void SetSeconds(double seconds) { seconds_in_day = seconds; }
  void SetF107A(double value) { f107a = value; }
  void SetF107(double value) { f107 = value; }
  void SetAP(double value) { ap[0] = value; }
};

constexpr double Rstar = DummyMSIS::GetRstar();
constexpr double gama = FGAtmosphere::SHRatio;
constexpr double beta = DummyMSIS::GetBeta();
constexpr double k = DummyMSIS::GetSutherlandConstant();
constexpr double psftopa = DummyMSIS::GetPSFtoPa();
constexpr double psftombar = psftopa/100.;
constexpr double psftoinhg = DummyMSIS::GetPSFtoInHg();

class FGMSISTest : public CxxTest::TestSuite, FGJSBBase
{
public:
  static constexpr double kmtoft = 1000. / fttom;
  static constexpr double gcm3_to_slugft3 = 1000. * kgtoslug / m3toft3;
  static constexpr double gtoslug = kgtoslug / 1000.;

  FGFDMExec fdmex;

  FGMSISTest() {
    auto atm = fdmex.GetAtmosphere();
    fdmex.GetPropertyManager()->Unbind(atm);
  }

  void testConstructor()
  {
    auto atm = DummyMSIS(&fdmex);

    double h = MSIS_alt[0]*kmtoft;

    atm.SetDay(MSIS_iyd[0]);
    atm.SetSeconds(MSIS_sec[0]);
    atm.in.altitudeASL = h;
    atm.in.GeodLatitudeDeg = MSIS_glat[0];
    atm.in.LongitudeDeg = MSIS_glon[0];
    atm.SetF107A(MSIS_f107a[0]);
    atm.SetF107(MSIS_f107[0]);
    atm.SetAP(MSIS_ap[0]);

    double T = KelvinToRankine(MSIS_T[0]);
    TS_ASSERT_EQUALS(atm.GetTemperatureSL(), 1.8);
    TS_ASSERT_EQUALS(atm.GetTemperature(), 1.8);
    TS_ASSERT_DELTA(atm.GetTemperature(h)/T, 1.0, 1E-5);
    TS_ASSERT_EQUALS(atm.GetTemperatureRatio(), 1.0);
    TS_ASSERT_DELTA(atm.GetTemperatureRatio(h)*1.8/T, 1.0, 1E-5);

    double rho = MSIS_rho[0]*gcm3_to_slugft3;
    TS_ASSERT_EQUALS(atm.GetDensitySL(), 1.0);
    TS_ASSERT_EQUALS(atm.GetDensity(), 0.0);
    TS_ASSERT_DELTA(atm.GetDensity(h)/rho, 1.0, 2E-4);
    TS_ASSERT_EQUALS(atm.GetDensityRatio(), 0.0);

    double R = Rstar / (MSIS_mair[0]*gtoslug);
    double P = rho*R*T;
    TS_ASSERT_EQUALS(atm.GetPressureSL(), 1.0);
    TS_ASSERT_EQUALS(atm.GetPressure(), 0.0);
    TS_ASSERT_DELTA(atm.GetPressure(h)/P, 1.0, 2E-4);
    TS_ASSERT_EQUALS(atm.GetPressureRatio(), 0.0);

    double a = sqrt(gama*R*T);
    TS_ASSERT_EQUALS(atm.GetSoundSpeedSL(), 1.0);
    TS_ASSERT_EQUALS(atm.GetSoundSpeed(), 0.0);
    TS_ASSERT_DELTA(atm.GetSoundSpeed(h)/a, 1.0, 1E-4);
    TS_ASSERT_EQUALS(atm.GetSoundSpeedRatio(), 0.0);

    TS_ASSERT_EQUALS(atm.GetDensityAltitude(), 0.0);
    TS_ASSERT_EQUALS(atm.GetPressureAltitude(), 0.0);

    TS_ASSERT_EQUALS(atm.GetAbsoluteViscosity(), 0.0);
    TS_ASSERT_EQUALS(atm.GetKinematicViscosity(), 0.0);
  }

  void testInitModel()
  {
    auto pm = fdmex.GetPropertyManager();
    auto theta_node = pm->GetNode("atmosphere/theta");
    auto sigma_node = pm->GetNode("atmosphere/sigma");
    auto delta_node = pm->GetNode("atmosphere/delta");
    auto a_ratio_node = pm->GetNode("atmosphere/a-ratio");

    auto atm = DummyMSIS(&fdmex);
    TS_ASSERT(atm.InitModel());

    for (unsigned int i=0; i<200; ++i) {
      double h = MSIS_alt[i]*kmtoft;

      atm.SetDay(MSIS_iyd[i]);
      atm.SetSeconds(MSIS_sec[i]);
      atm.in.altitudeASL = h;
      atm.in.GeodLatitudeDeg = MSIS_glat[i];
      atm.in.LongitudeDeg = MSIS_glon[i];
      atm.SetF107A(MSIS_f107a[i]);
      atm.SetF107(MSIS_f107[i]);
      atm.SetAP(MSIS_ap[i]);

      double T = KelvinToRankine(MSIS_T[i]);
      TS_ASSERT_DELTA(atm.GetTemperature(h)/T, 1.0, 1E-4);
      TS_ASSERT_EQUALS(atm.GetTemperatureRatio(), 1.0);
      TS_ASSERT_EQUALS(theta_node->getDoubleValue(), 1.0);

      double rho = MSIS_rho[i]*gcm3_to_slugft3;
      TS_ASSERT_DELTA(atm.GetDensity(h)/rho, 1.0, 5E-4);
      TS_ASSERT_EQUALS(atm.GetDensityRatio(), 1.0);
      TS_ASSERT_EQUALS(sigma_node->getDoubleValue(), 1.0);

      double R = Rstar / (MSIS_mair[i]*gtoslug);
      double P = rho*R*T;
      TS_ASSERT_DELTA(atm.GetPressure(h)/P, 1.0, 5E-4);
      TS_ASSERT_EQUALS(atm.GetPressureRatio(), 1.0);
      TS_ASSERT_EQUALS(delta_node->getDoubleValue(), 1.0);

      double a = sqrt(gama*R*T);
      TS_ASSERT_DELTA(atm.GetSoundSpeed(h)/a, 1.0, 1E-4);
      TS_ASSERT_EQUALS(atm.GetSoundSpeedRatio(), 1.0);
      TS_ASSERT_EQUALS(a_ratio_node->getDoubleValue(), 1.0);
    }
  }

  void testRun()
  {
    auto pm = fdmex.GetPropertyManager();
    auto T_node = pm->GetNode("atmosphere/T-R");
    auto rho_node = pm->GetNode("atmosphere/rho-slugs_ft3");
    auto P_node = pm->GetNode("atmosphere/P-psf");
    auto a_node = pm->GetNode("atmosphere/a-fps");
    auto T0_node = pm->GetNode("atmosphere/T-sl-R");
    auto rho0_node = pm->GetNode("atmosphere/rho-sl-slugs_ft3");
    auto a0_node = pm->GetNode("atmosphere/a-sl-fps");
    auto theta_node = pm->GetNode("atmosphere/theta");
    auto sigma_node = pm->GetNode("atmosphere/sigma");
    auto delta_node = pm->GetNode("atmosphere/delta");
    auto a_ratio_node = pm->GetNode("atmosphere/a-ratio");
    auto density_altitude_node = pm->GetNode("atmosphere/density-altitude");
    auto pressure_altitude_node = pm->GetNode("atmosphere/pressure-altitude");

    auto atm = DummyMSIS(&fdmex);
    TS_ASSERT(atm.InitModel());

    for (unsigned int i=0; i<200; ++i) {
      double h = MSIS_alt[i]*kmtoft;

      atm.SetDay(MSIS_iyd[i]);
      atm.SetSeconds(MSIS_sec[i]);
      atm.in.altitudeASL = h;
      atm.in.GeodLatitudeDeg = MSIS_glat[i];
      atm.in.LongitudeDeg = MSIS_glon[i];
      atm.SetF107A(MSIS_f107a[i]);
      atm.SetF107(MSIS_f107[i]);
      atm.SetAP(MSIS_ap[i]);

      TS_ASSERT(atm.Run(false) == false);

      double T = KelvinToRankine(MSIS_T[i]);
      double T_SL = atm.GetTemperatureSL();
      double T0 = atm.GetTemperature(0.0);
      TS_ASSERT_DELTA(atm.GetTemperature()/T, 1.0, 1E-4);
      TS_ASSERT_DELTA(T_node->getDoubleValue()/T, 1.0, 1E-4);
      TS_ASSERT_EQUALS(T_SL, T0);
      TS_ASSERT_EQUALS(T0_node->getDoubleValue(), T_SL);
      TS_ASSERT_DELTA(atm.GetTemperatureRatio()*T_SL/T, 1.0, 1E-4);
      TS_ASSERT_DELTA(theta_node->getDoubleValue()*T_SL/T, 1.0, 1E-4);

      double rho = MSIS_rho[i]*gcm3_to_slugft3;
      double rho_SL = atm.GetDensitySL();
      double rho0= atm.GetDensity(0.0);
      TS_ASSERT_DELTA(atm.GetDensity()/rho, 1.0, 5E-4);
      TS_ASSERT_DELTA(rho_node->getDoubleValue()/rho, 1.0, 5E-4);
      TS_ASSERT_EQUALS(rho_SL, rho0);
      TS_ASSERT_EQUALS(rho0_node->getDoubleValue(), rho_SL);
      TS_ASSERT_DELTA(atm.GetDensityRatio()*rho_SL/rho, 1.0, 5E-4);
      TS_ASSERT_DELTA(sigma_node->getDoubleValue()*rho_SL/rho, 1.0, 5E-4);

      double R = Rstar / (MSIS_mair[i]*gtoslug);
      double P = rho*R*T;
      double P_SL = atm.GetPressureSL();
      double P0 = atm.GetPressure(0.0);
      TS_ASSERT_DELTA(atm.GetPressure()/P, 1.0, 5E-4);
      TS_ASSERT_DELTA(P_node->getDoubleValue()/P, 1.0, 5E-4);
      TS_ASSERT_EQUALS(P_SL, P0);
      TS_ASSERT_DELTA(atm.GetPressureRatio()*P_SL/P, 1.0, 5E-4);
      TS_ASSERT_DELTA(delta_node->getDoubleValue()*P_SL/P, 1.0, 5E-4);

      double a = sqrt(gama*R*T);
      double a_SL = atm.GetSoundSpeedSL();
      double a0 = atm.GetSoundSpeed(0.0);
      TS_ASSERT_DELTA(atm.GetSoundSpeed()/a, 1.0, 1E-4);
      TS_ASSERT_DELTA(a_node->getDoubleValue()/a, 1.0, 1E-4);
      TS_ASSERT_EQUALS(a_SL, a0);
      TS_ASSERT_EQUALS(a0_node->getDoubleValue(), a_SL);
      TS_ASSERT_DELTA(atm.GetSoundSpeedRatio()*a_SL/a, 1.0, 1E-4);
      TS_ASSERT_DELTA(a_ratio_node->getDoubleValue()*a_SL/a, 1.0, 1E-4);

      double mu = beta*T*sqrt(T)/(k+T);
      double nu = mu/rho;
      TS_ASSERT_DELTA(atm.GetAbsoluteViscosity(), mu, 1E-4);
      TS_ASSERT_DELTA(atm.GetKinematicViscosity()/nu, 1.0, 5E-4);

      TS_ASSERT_EQUALS(density_altitude_node->getDoubleValue(), atm.GetDensityAltitude());
      TS_ASSERT_EQUALS(pressure_altitude_node->getDoubleValue(), atm.GetPressureAltitude());
    }
  }
};
