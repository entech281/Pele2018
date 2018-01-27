#include <WPILib.h>

#include "DriveSubsystem.h"
#include "RobotConstants.h"

DriveSubsystem::DriveSubsystem(EntechRobot *pRobot, std::string name)
  : RobotSubsystem(pRobot, name)
  , m_joystick(NULL)
  , m_frmotor(NULL)
  , m_flmotor(NULL)
  , m_rrmotor(NULL)
  , m_rlmotor(NULL)
  , m_robotDrive(NULL)
  , m_ahrs(NULL)
  , m_fieldAbsolute(false)
  , m_toggleFieldAbsoluteButton(NULL)
{
}

DriveSubsystem::~DriveSubsystem() {}

void DriveSubsystem::ToggleFieldAbsoluteDriving(void)
{
	SetFieldAbsoluteDriving(!m_fieldAbsolute);
}

void DriveSubsystem::SetFieldAbsoluteDriving(bool active)
{
	m_fieldAbsolute = active;
}

/********************************** Init Routines **********************************/

void DriveSubsystem::RobotInit()
{
    m_joystick = new Joystick(0);

    m_flmotor = new Victor(c_flmotor_PWMid);
    m_frmotor = new Victor(c_frmotor_PWMid);
    m_rlmotor = new Victor(c_rlmotor_PWMid);
    m_rrmotor = new Victor(c_rrmotor_PWMid);

    m_flmotor->SetInverted(c_kflmotor_inversed);
    m_frmotor->SetInverted(c_kfrmotor_inversed);
    m_rlmotor->SetInverted(c_krlmotor_inversed);
    m_rrmotor->SetInverted(c_krrmotor_inversed);

    m_robotDrive = new frc::MecanumDrive(*m_flmotor, *m_rlmotor, *m_frmotor, *m_rrmotor);
    m_robotDrive->SetSafetyEnabled(false);

    try {
    	m_ahrs = new AHRS(SerialPort::Port::kUSB);
        DriverStation::ReportError("NavX FOUND");
        m_ahrs->Reset();
        if (m_ahrs->IsCalibrating()) {
            Wait(0.25);
        }
        m_ahrs->ZeroYaw();
    } catch (std::exception& ex) {
        m_ahrs = NULL;
    }

    m_toggleFieldAbsoluteButton = new OperatorButton(m_joystick, c_jsfieldAbs_BTNid);
}

void DriveSubsystem::DisabledInit() {}

void DriveSubsystem::TeleopInit() {}

void DriveSubsystem::AutonomousInit() {}

void DriveSubsystem::TestInit() {}

/********************************** Periodic Routines **********************************/

void DriveSubsystem::DisabledPeriodic()
{
    m_robotDrive->DriveCartesian(0.0, 0.0, 0.0, 0.0);
}

void DriveSubsystem::TeleopPeriodic()
{
    double jsX, jsY, jsT, gyroAngle;

    jsX = 0.0;
    jsY = 0.0;
    if (m_joystick) {
        jsX = m_joystick->GetX();
        jsY = m_joystick->GetY();
    }

    /* Rotate the robot only if the trigger being held. */
    jsT = 0.0;
    if (m_joystick->GetTrigger()) {
        jsT = m_joystick->GetTwist();
    }

    if (m_toggleFieldAbsoluteButton->Get() == OperatorButton::kJustPressed) {
    	ToggleFieldAbsoluteDriving();
    }

    gyroAngle = 0.0;
    if (m_fieldAbsolute && m_ahrs) {
    	gyroAngle = m_ahrs->GetAngle();
    }

    /* Move the robot */
    m_robotDrive->DriveCartesian(jsX, jsY, jsT, gyroAngle);
}

void DriveSubsystem::AutonomousPeriodic() {}

void DriveSubsystem::TestPeriodic() {}

void DriveSubsystem::UpdateDashboard(void)
{
    if (m_ahrs) {
        SmartDashboard::PutData("NavX", m_ahrs);
        SmartDashboard::PutString("NavX Exists", "YES");
        SmartDashboard::PutNumber("NavX GetAngle()", m_ahrs->GetAngle()); //total accumulated yaw angle, 360+
        SmartDashboard::PutNumber("NavX GetYaw()", m_ahrs->GetYaw()); //-180 to 180 degrees
    } else {
        SmartDashboard::PutString("NavX Exists", "NO");
    }
    SmartDashboard::PutNumber("Drive FieldAbsolute", m_fieldAbsolute);
}
