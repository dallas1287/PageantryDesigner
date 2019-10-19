#include "utils.h"

void convertTransformMatrix(aiMatrix4x4& in, QMatrix4x4& out)
{
	float vals[16] =
	{
		in.a1, in.a2, in.a3, in.a4,
		in.b1, in.b2, in.b3, in.b4,
		in.c1, in.c2, in.c3, in.c4,
		in.d1, in.d2, in.d3, in.d4
	};

	out = QMatrix4x4(vals);
}

QMatrix4x4 convertTransformMatrix(aiMatrix4x4& in)
{
	float vals[16] =
	{
		in.a1, in.a2, in.a3, in.a4,
		in.b1, in.b2, in.b3, in.b4,
		in.c1, in.c2, in.c3, in.c4,
		in.d1, in.d2, in.d3, in.d4
	};

	return QMatrix4x4(vals);
}