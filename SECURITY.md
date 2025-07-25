# 🔐 Security Policy

## Supported Versions

We maintain security updates for the following versions:

| Version | Supported          |
| ------- | ------------------ |
| 5.1.x   | ✅ |
| 5.0.x   | ❌ |
| 4.0.x   | ✅ |
| < 4.0   | ❌ |

---

## 📣 Reporting a Vulnerability

If you discover a security vulnerability within this project:

1. **Report immediately via email** to: [security@savoyresearch.org](mailto:security@savoyresearch.org)
2. **Include:**
   - Detailed description of the vulnerability
   - Steps to reproduce
   - Potential impact
   - Suggested remediation if known

🔁 **Response Expectations:**

- **Initial acknowledgment**: Within 48 hours
- **Assessment & triage**: Within 5 business days
- **Resolution timeline**: Provided upon validation
- **Public disclosure**: Coordinated with the reporter after fix deployment

---

## 🔒 Encryption & Secure Communication

To ensure secure collaboration:

- **PGP/GPG encryption** is encouraged for all sensitive communication.  
  Our public security PGP key is available [here](link-to-your-public-pgp-key).

- For internal team communication:
  - Use **end-to-end encrypted channels** (e.g. Signal, WhatsApp with 2FA).
  - Slack channels handling credentials should be **restricted and monitored**.

- **Secrets Management:**
  - Never commit API keys, credentials, or certificates into the Git repository.
  - Use environment variables or secret management tools (e.g. GitHub Secrets, Vault).

---

## 🛡️ Software & Cybersecurity Best Practices

We actively track modern cybersecurity practices including:

- OWASP Top 10 mitigation strategies
- Regular dependency updates and vulnerability scanning
- Zero-trust principles in system architecture
- Firmware signing and secure boot considerations for embedded systems
- Secure over-the-air (OTA) update protocols for hardware deployments

---

## 🚨 If You Notice Suspicious Activity

If you observe abnormal repository activity, suspicious commits, or potential breaches:

1. Alert the Security Team immediately via [security@savoyresearch.org](mailto:security@savoyresearch.org).
2. Include screenshots, logs, or relevant evidence.

---

**Thank you for helping keep our project and community secure.**

— **Savoy Research Institute | Security & Compliance Team**
