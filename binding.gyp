{
  "targets": [
    {
      "target_name": "logigatorSimulation",
	  "include_dirs": [
		"src",
		"src/components",
		"<!(node -e \"require('nan')\")"
	  ],
      "sources": [
		"src/project.cpp",
		"src/barrier.h",
		"src/board.cpp",
		"src/board.h",
		"src/component.h",
		"src/events.h",
		"src/input.h",
		"src/link.h",
		"src/output.cpp",
		"src/output.h",
		"src/spinlock_barrier.h",
		"src/components/and.h",
		"src/components/clk.h",
		"src/components/delay.h",
		"src/components/half_addr.h",
		"src/components/full_addr.h",
		"src/components/not.h",
		"src/components/or.h",
		"src/components/user_input.h",
		"src/components/rom.h",
		"src/components/xor.h"
	  ]
    }
  ]
}
