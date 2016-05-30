#pragma once

class NetworkTask {

	PROGRAM_STATE taskType;

	public:
		NetworkTask(const PROGRAM_STATE& m_taskType) {
			taskType = m_taskType;
		}

		PROGRAM_STATE getTaskType() {
			return taskType;
		}

};
