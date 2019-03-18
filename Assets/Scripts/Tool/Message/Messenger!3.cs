using System;
using System.Collections.Generic;

namespace DMessager
{
	public static class Messenger<M, T, U>
	{
	    private static Dictionary<M, Delegate> eventTable;
	
	    static Messenger()
	    {
	        Messenger<M, T, U>.eventTable = MessengerInternal<M>.eventTable;
	    }
	
	    public static void addListener(M messageType, OnMessageDelegate<M, T, U> handler)
	    {
	        MessengerInternal<M>.onAddListener(messageType, handler);
	        Messenger<M, T, U>.eventTable[messageType] = (OnMessageDelegate<M, T, U>) Delegate.Combine((OnMessageDelegate<M, T, U>) Messenger<M, T, U>.eventTable[messageType], handler);
	    }
	
	    public static void postMessage(M messageType, T arg1, U arg2)
	    {
	        Delegate delegate2;
	        if (Messenger<M, T, U>.eventTable.TryGetValue(messageType, out delegate2))
	        {
	            OnMessageDelegate<M, T, U> delegate3 = delegate2 as OnMessageDelegate<M, T, U>;
	            if (delegate3 != null)
	            {
	                delegate3(arg1, arg2);
	            }
	        }
	    }
	
	    public static void removeListener(M messageType, OnMessageDelegate<M, T, U> handler)
	    {
	        if (Messenger<M, T, U>.eventTable.ContainsKey(messageType))
	        {
	            Messenger<M, T, U>.eventTable[messageType] = (OnMessageDelegate<M, T, U>) Delegate.Remove((OnMessageDelegate<M, T, U>) Messenger<M, T, U>.eventTable[messageType], handler);
	            MessengerInternal<M>.onRemoveListener(messageType);
	        }
	    }
	}
}

