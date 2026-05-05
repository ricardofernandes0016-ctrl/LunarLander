#ifndef PIDCONTROLLER_HPP
#define PIDCONTROLLER_HPP

// A simple discrete PID controller.
//
// Usage:
//   PIDController pid(kp, ki, kd);
//   float output = pid.calculate(setpoint, currentValue, dt);
//
// The output is clamped to [outMin, outMax] to prevent wind-up and
// to keep the thrust power in the [0, 1] range expected by the ship.
class PIDController {
public:
    // Suggested initial gains for Lunar Lander:
    //   gravity = 80 units/s²,  thrustMagnitude = 140 units/s²
    //   Net upward authority = 140 - 80 = 60 units/s²
    //
    //   Kp = 0.05  — gentle proportional response (avoids oscillation)
    //   Ki = 0.01  — slow integral to correct steady-state drift
    //   Kd = 0.08  — derivative dampens sudden speed changes
    PIDController(float kp = 0.05f, float ki = 0.01f, float kd = 0.08f,
                  float outMin = 0.0f, float outMax = 1.0f)
        : m_kp(kp), m_ki(ki), m_kd(kd),
          m_outMin(outMin), m_outMax(outMax),
          m_integral(0.f), m_previousError(0.f) {}

    // Calculates the control output for one time step.
    // setpoint     — desired value (e.g. target descent speed)
    // currentValue — measured value (e.g. current vertical velocity)
    // dt           — elapsed time in seconds since last call
    float calculate(float setpoint, float currentValue, float dt) {
        if (dt <= 0.f) return 0.f;

        float error = setpoint - currentValue;

        // Accumulate integral, but clamp to avoid wind-up
        m_integral += error * dt;
        float integralClamped = clamp(m_integral, m_outMin / m_ki, m_outMax / m_ki);
        m_integral = integralClamped;

        float derivative = (error - m_previousError) / dt;
        m_previousError = error;

        float output = m_kp * error + m_ki * m_integral + m_kd * derivative;
        return clamp(output, m_outMin, m_outMax);
    }

    // Resets accumulated state (call when autopilot is re-enabled after a pause)
    void reset() {
        m_integral = 0.f;
        m_previousError = 0.f;
    }

private:
    float m_kp, m_ki, m_kd;
    float m_outMin, m_outMax;
    float m_integral;
    float m_previousError;

    static float clamp(float v, float lo, float hi) {
        return v < lo ? lo : (v > hi ? hi : v);
    }
};

#endif
