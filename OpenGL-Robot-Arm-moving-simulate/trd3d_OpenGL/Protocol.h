#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

namespace MY_PROTOCOL
{
	enum
	{
		EOP_UP,
		EOP_DOWN,
		EOP_LEFT,
		EOP_RIGHT
	};

	struct _st_Data 
	{
		int operator_id;     // operator id
		int data_len;        // data length
		char *data;          // data
	};
}

#endif // _PROTOCOL_H_