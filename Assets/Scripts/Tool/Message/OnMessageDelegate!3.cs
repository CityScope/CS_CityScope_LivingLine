using System;
using System.Runtime.CompilerServices;

namespace DMessager
{
	public delegate void OnMessageDelegate<M, T, U>(T arg1, U arg2);
}

