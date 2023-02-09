## Important conventions to follow:

- Always include header guards
- #include dependencies
- Have every important function and its implementation
- Watch out for globals and conflicts
- Packetize initializations (almost always in main) as init functions
- Keep in mind which ports are used by the microcontroller and consider adding them to the init function
- Have a CLEAR description in the start so that minimal user effort is required for understanding