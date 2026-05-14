/*
1. Startup & Initialization (logs 01–09)

2. Debug / Anti-analysis (10–17)

3. Environment detection (18–21)

4. Persistence & Forking (22–23)

5. Seed & Key material (24–31)

6. Network setup & listeners (32–39)

7. Runtime delays & jitter (40, 58)

8. Authentication (41–46)

9. Command execution (47–49)

10. Payload & shellcode (50–54)

11. Anti-forensics / Deception (55–60)

12. Shutdown / Cleanup (61–62)

01. Program started.
02. Logfile Selected
03. Initializing seed.
04. Found Seed.
05. Using Seed:
06. Writing Seed.
07. Saved Seed.
08. Decoding Run Time Key
09. Encoding Run Time Key

10. Debugging Detected
11. Anti-debugging triggered
12. Debugging Mode Started
13. Debugging Mode Ended
14. Verbose Level
15. Debug File Open
16. Debug File Close
17. Debug SHA512

18. VM detected — altering behavior
19. Sandbox environment detected
20. Debug is Win
21. Debug is Linux
22. Debug is Mac
23. Debug is Version

24. Forking into background
25. Patching Executable

26. Binding to host
27. Listening to port
28. Starting Transparent Proxy
29. Stopping Transparent Proxy
30. Starting Magic 8 Ball
31. Stopping Magic 8 Ball
32. Starting Connect Back Shell
33. Stopping Connect Back Shell
34. Connecting to host
35. Connecting to port
36. Connection from tcp
37. Connection from pipe

38. Received Signal
39. Sleeping for [X] seconds
40. Jitter applied: +/- [X] ms

41. User login
42. Password Correct
43. Password Incorrect
44. Password Change
45. User unknown

46. Exec Command
47. Program shell command
48. Using name transformation

49. Encrypted payload decrypted
50. Executing Shellcode
51. Executing from memory
52. Spawning Shell
53. Killing parent process

54. Starting Polymorphic Behavior
55. Shooting Penguins
56. Decoy traffic generated
57. Timestamp modified
58. Fake error generated

59. Debug SEED 


*/

char *f_errors[] = {
    // ========== MEMORY ACCESS VIOLATIONS ==========
    "Access violation at address: ",
    "Access granted at address: ",
    "Attempted to read/write protected memory: ",
    "Permitted to read protected memory: ",
    "Permitted to write protected memory: ",
    "(Access Violation) Exception at: ",
    "Null pointer dereference at: ",
    "Invalid memory reference instruction at: ",

    // ========== MEMORY ALLOCATION ==========
    "Failed to allocate RWX memory region: ",
    "Failed to allocate RW memory region: ",
    "Success to allocate RW memory region: ",
    "Success to allocate RWX memory region: ",
    "Failed setting +X to memory: ",
    "Allocation size exceeds maximum: ",

    // ========== HEAP / STACK CORRUPTION ==========
    "Heap corruption detected near: ",
    "Double free detected on address: ",
    "Use-after-free access at: ",
    "Stack overflow near variable: ",
    "Heap block overrun by: ",
    "Free called on invalid pointer: ",
    "Memory leak threshold exceeded: ",

    // ========== CRYPTOGRAPHY ==========
    "Crypto functions initiated at: ",
    "Crypto function hash successful: ",
    "Crypto function hash failed: ",
    "Crypto function hash: ",
    "No valid context for cryptographic operation on ",
    "Invalid key length supplied to: ",
    "Random number generator entropy depleted",
    "Certificate validation failed: ",
    "Decryption padding mismatch: ",
    "Secure channel negotiation timeout",

    // ========== SYMBOL / LINKER ==========
    "Cannot resolve external symbol: ",
    "Resolve external symbol: ",
    "Missing entry point: ",
    "Unhandled exception in dynamic linker",
    "Unwind failed: corrupted frame pointer",
    "TLS (Thread Local Storage) init failed",
    "Relocation truncated to fit: ",

    // ========== INTEGRITY (CRC / RTTI) ==========
    "CRC mismatch in loaded module: ",
    "CRC match in loaded module: ",
    "RTTI corrupted or missing for vtable",
    "Stack cookie mismatch during unwind ",

    // ========== PROCESS / THREAD ==========
    "Thread deadlock detected on mutex: ",
    "Race condition suspected at: ",
    "Failed to create process: ",
    "Process terminated unexpectedly with code: ",
    "Thread pool starvation: ",
    "Signal handler reentrancy detected (signal: )",

    // ========== FILE I/O ==========
    "Failed to open: ",
    "Failed to close: ",
    "Failed to find: ",
    "File descriptor leak detected (fd: )",
    "Write to read-only file: ",
    "File lock acquisition timeout: ",
    "Directory traversal attempt blocked: ",

    // ========== NETWORKING ==========
    "Host lookup: ",
    "Connecting to: ",
    "Connection from: ",
    "Packet from: ",
    "Packet to: ",
    "Opening port: ",
    "Binding host: ",
    "TCP retransmission limit reached for: ",
    "DNS lookup: ",
    "Socket bind failed: address already in use",
    "Connection reset by peer: ",
    "TLS handshake failure: cipher mismatch",

    // ========== IPC ==========
    "Inter-process communication buffer overflow",

    // ========== CALLBACKS ==========
    "callback failed to initialize at: ",

    // ========== TIMING ==========
    "Start time: ",
    "End time: "
};

char *f_domains[] = {
    // 1. Search & Advertising
    "google.com",

    // 2. Cloud Infrastructure (IaaS)
    "aws.com",
    "azure.com",
    "gcp.com",              // Google Cloud Platform

    // 3. CDN & DDoS Protection
    "cloudflare.com",
    "fastly.com",
    "akamai.com",

    // 4. Operating Systems / Hardware
    "apple.com",
    "microsoft.com",

    // 5. Social Media
    "meta.com",             // Facebook/Instagram
    "twitter.com",
    "tiktok.com",

    // 6. E-commerce / Retail
    "amazon.com",
    "walmart.com",

    // 7. Enterprise SaaS
    "salesforce.com",
    "oracle.com"
};

char log_message[] = 
  "Log line 1",
  "log line 2",
  "log line 3", 
