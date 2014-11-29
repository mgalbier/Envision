/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#pragma once

#include "../../oodebug_api.h"

namespace OODebug {


/**
 * Defines all constant values as in:
 * https://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html
 *
 * Most of this file can be generated by regex expressions.
 */
class OODEBUG_API Protocol
{
	public:
		static const QByteArray handshake;

		// http://docs.oracle.com/javase/7/docs/technotes/guides/jpda/jdwp-spec.html
		// http://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html
		enum class CommandSet : qint8 {
			VirtualMachine = 1,
			ReferenceType = 2,
			ClassType = 3,
			ArrayType = 4,
			InterfaceType = 5,
			Method = 6,
			Field = 8,
			ObjectReference = 9,
			StringReference = 10,
			ThreadReference = 11,
			ThreadGroupReference = 12,
			ArrayReference = 13,
			ClassLoaderReference = 14,
			EventRequest = 15,
			StackFrame = 16,
			ClassObjectReference = 17,
			Event = 64
		};

		// The following values can be generated with regex:
		// Copy the sublist of a command set from:
		// http://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html
		// to a text editor (e.g. Kate) search for "(^[^ ]+) \(([0-9]+)\)\n" and replace with "\1 = \2,\n"
		// minimal manual interaction might be needed
		enum class VirtualMachineCommands : qint8 {
			Version = 1,
			ClassesBySignature = 2,
			AllClasses = 3,
			AllThreads = 4,
			TopLevelThreadGroups = 5,
			Dispose = 6,
			IDSizes = 7,
			Suspend = 8,
			Resume = 9,
			Exit = 10,
			CreateString = 11,
			Capabilities = 12,
			ClassPaths = 13,
			DisposeObjects = 14,
			HoldEvents = 15,
			ReleaseEvents = 16,
			CapabilitiesNew = 17,
			RedefineClasses = 18,
			SetDefaultStratum = 19,
			AllClassesWithGeneric = 20,
			InstanceCounts = 21
		};

		enum class ReferenceTypeCommands : int {
			Signature = 1,
			ClassLoader = 2,
			Modifiers = 3,
			Fields = 4,
			Methods = 5,
			GetValues = 6,
			SourceFile = 7,
			NestedTypes = 8,
			Status = 9,
			Interfaces = 10,
			ClassObject = 11,
			SourceDebugExtension = 12,
			SignatureWithGeneric = 13,
			FieldsWithGeneric = 14,
			MethodsWithGeneric = 15,
			Instances = 16,
			ClassFileVersion = 17,
			ConstantPool = 18
		};

		enum class ClassTypeCommands : int {
			Superclass = 1,
			SetValues = 2,
			InvokeMethod = 3,
			NewInstance = 4
		};

		enum class ArrayTypeCommands : int {
			NewInstance = 1
		};

		enum class InterfaceTypeCommands : int {
			// Has no commands, just here for consistency.
		};

		enum class MethodCommands : int {
			LineTable = 1,
			VariableTable = 2,
			Bytecodes = 3,
			IsObsolete = 4,
			VariableTableWithGeneric = 5
		};

		enum class FieldCommands : int {
			// Has no commands, just here for consistency.
		};

		enum class ObjectReferenceCommands : int {
			ReferenceType = 1,
			GetValues = 2,
			SetValues = 3,
			MonitorInfo = 5,
			InvokeMethod = 6,
			DisableCollection = 7,
			EnableCollection = 8,
			IsCollected = 9,
			ReferringObjects = 10
		};

		enum class StringReferenceCommands : int {
			Value = 1
		};

		enum class ThreadReferenceCommands : int {
			Name = 1,
			Suspend = 2,
			Resume = 3,
			Status = 4,
			ThreadGroup = 5,
			Frames = 6,
			FrameCount = 7,
			OwnedMonitors = 8,
			CurrentContendedMonitor = 9,
			Stop = 10,
			Interrupt = 11,
			SuspendCount = 12,
			OwnedMonitorsStackDepthInfo = 13,
			ForceEarlyReturn = 14
		};

		enum class ThreadGroupReferenceCommands : int {
			Name = 1,
			Parent = 2,
			Children = 3
		};

		enum class ArrayReferenceCommands : int {
			Length = 1,
			GetValues = 2,
			SetValues = 3
		};

		enum class ClassLoaderReferenceCommands : int {
			VisibleClasses = 1
		};

		enum class EventRequestCommands : int {
			Set = 1,
			Clear = 2,
			ClearAllBreakpoints = 3
		};

		enum class StackFrameCommands : int {
			GetValues = 1,
			SetValues = 2,
			ThisObject = 3,
			PopFrames = 4
		};

		enum class ClassObjectReferenceCommands : int {
			ReflectedType = 1
		};

		enum class EventCommands : int {
			Composite = 100
		};

		// Generated with regex from:
		// https://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html#JDWP_Error
		// Copy to text editor (tab enabled) replace "(^[^\t]+)\t([^\t]+)\t([^\n]+)\n" with "/** \3 */\n\1 = \2,\n"
		enum class Error : qint16 {
			/** No error has occurred.   */
			NONE = 0,
			/** Passed thread is null, is not a valid thread or has exited. */
			INVALID_THREAD = 10,
			/** Thread group invalid. */
			INVALID_THREAD_GROUP = 11,
			/** Invalid priority.   */
			INVALID_PRIORITY = 12,
			/** If the specified thread has not been suspended by an event.   */
			THREAD_NOT_SUSPENDED = 13,
			/** Thread already suspended.   */
			THREAD_SUSPENDED = 14,
			/** Thread has not been started or is now dead.   */
			THREAD_NOT_ALIVE = 15,
			/** If this reference type has been unloaded and garbage collected.   */
			INVALID_OBJECT = 20,
			/** Invalid class.   */
			INVALID_CLASS = 21,
			/** Class has been loaded but not yet prepared.   */
			CLASS_NOT_PREPARED = 22,
			/** Invalid method.   */
			INVALID_METHODID = 23,
			/** Invalid location.   */
			INVALID_LOCATION = 24,
			/** Invalid field.   */
			INVALID_FIELDID = 25,
			/** Invalid jframeID.   */
			INVALID_FRAMEID = 30,
			/** There are no more Java or JNI frames on the call stack.   */
			NO_MORE_FRAMES = 31,
			/** Information about the frame is not available.   */
			OPAQUE_FRAME = 32,
			/** Operation can only be performed on current frame.   */
			NOT_CURRENT_FRAME = 33,
			/** The variable is not an appropriate type for the function used.   */
			TYPE_MISMATCH = 34,
			/** Invalid slot.   */
			INVALID_SLOT = 35,
			/** Item already set.   */
			DUPLICATE = 40,
			/** Desired element not found.   */
			NOT_FOUND = 41,
			/** Invalid monitor.   */
			INVALID_MONITOR = 50,
			/** This thread doesn't own the monitor.   */
			NOT_MONITOR_OWNER = 51,
			/** The call has been interrupted before completion.   */
			INTERRUPT = 52,
			/** The virtual machine attempted to read a class file and determined that the file is malformed
			 *  or otherwise cannot be interpreted as a class file.   */
			INVALID_CLASS_FORMAT = 60,
			/** A circularity has been detected while initializing a class.   */
			CIRCULAR_CLASS_DEFINITION = 61,
			/** The verifier detected that a class file, though well formed,
			 * contained some sort of internal inconsistency or security problem.   */
			FAILS_VERIFICATION = 62,
			/** Adding methods has not been implemented.   */
			ADD_METHOD_NOT_IMPLEMENTED = 63,
			/** Schema change has not been implemented.   */
			SCHEMA_CHANGE_NOT_IMPLEMENTED = 64,
			/** The state of the thread has been modified, and is now inconsistent.   */
			INVALID_TYPESTATE = 65,
			/** A direct superclass is different for the new class version,
			 * or the set of directly implemented interfaces is different and canUnrestrictedlyRedefineClasses is false.*/
			HIERARCHY_CHANGE_NOT_IMPLEMENTED = 66,
			/** The new class version does not declare a method declared in the old class version
			 * and canUnrestrictedlyRedefineClasses is false.   */
			DELETE_METHOD_NOT_IMPLEMENTED = 67,
			/** A class file has a version number not supported by this VM.   */
			UNSUPPORTED_VERSION = 68,
			/** The class name defined in the new class file is different from the name in the old class object.   */
			NAMES_DONT_MATCH = 69,
			/** The new class version has different modifiers and and canUnrestrictedlyRedefineClasses is false.   */
			CLASS_MODIFIERS_CHANGE_NOT_IMPLEMENTED = 70,
			/** A method in the new class version has different modifiers than its counterpart in the old class version
			 * and canUnrestrictedlyRedefineClasses is false.   */
			METHOD_MODIFIERS_CHANGE_NOT_IMPLEMENTED = 71,
			/** The functionality is not implemented in this virtual machine.   */
			NOT_IMPLEMENTED = 99,
			/** Invalid pointer.   */
			NULL_POINTER = 100,
			/** Desired information is not available.   */
			ABSENT_INFORMATION = 101,
			/** The specified event type id is not recognized.   */
			INVALID_EVENT_TYPE = 102,
			/** Illegal argument.   */
			ILLEGAL_ARGUMENT = 103,
			/** The function needed to allocate memory and no more memory was available for allocation.   */
			OUT_OF_MEMORY = 110,
			/** Debugging has not been enabled in this virtual machine. JVMTI cannot be used.   */
			ACCESS_DENIED = 111,
			/** The virtual machine is not running.   */
			VM_DEAD = 112,
			/** An unexpected internal error has occurred.   */
			INTERNAL = 113,
			/** The thread being used to call this function is not attached to the virtual machine.
			 * Calls must be made from attached threads.   */
			UNATTACHED_THREAD = 115,
			/** object type id or class tag.   */
			INVALID_TAG = 500,
			/** Previous invoke not complete.   */
			ALREADY_INVOKING = 502,
			/** Index is invalid.   */
			INVALID_INDEX = 503,
			/** The length is invalid.   */
			INVALID_LENGTH = 504,
			/** The string is invalid.   */
			INVALID_STRING = 506,
			/** The class loader is invalid.   */
			INVALID_CLASS_LOADER = 507,
			/** The array is invalid.   */
			INVALID_ARRAY = 508,
			/** Unable to load the transport.   */
			TRANSPORT_LOAD = 509,
			/** Unable to initialize the transport.   */
			TRANSPORT_INIT = 510,
			/**   */
			NATIVE_METHOD = 511,
			/** The count is invalid */
			INVALID_COUNT = 512
		};

		// The following values can be generated with regex:
		// Copy the table (e.g. http://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html#JDWP_EventKind)
		// to a text editor (e.g. Kate) search for "(^[^\t]+)\t([^\t]+)[^\n]+\n" and replace with: "\1 = \2,\n"
		// minimal manual interaction might be needed
		enum class EventKind : qint8 {
			SINGLE_STEP = 1,
			BREAKPOINT = 2,
			FRAME_POP = 3,
			EXCEPTION = 4,
			USER_DEFINED = 5,
			THREAD_START = 6,
			THREAD_DEATH = 7,
			THREAD_END = 7,
			CLASS_PREPARE = 8,
			CLASS_UNLOAD = 9,
			CLASS_LOAD = 10,
			FIELD_ACCESS = 20,
			FIELD_MODIFICATION = 21,
			EXCEPTION_CATCH = 30,
			METHOD_ENTRY = 40,
			METHOD_EXIT = 41,
			METHOD_EXIT_WITH_RETURN_VALUE = 42,
			MONITOR_CONTENDED_ENTER = 43,
			MONITOR_CONTENDED_ENTERED = 44,
			MONITOR_WAIT = 45,
			MONITOR_WAITED = 46,
			VM_START = 90,
			VM_INIT = 90,
			VM_DEATH = 99,
			VM_DISCONNECTED = 100
		};

		enum class ThreadStatus : qint32 {
			ZOMBIE = 0,
			RUNNING = 1,
			SLEEPING = 2,
			MONITOR = 3,
			WAIT = 4
		};

		enum class SuspendStatus : qint32 {
			SUSPEND_STATUS_SUSPENDED = 0x1
		};

		enum class ClassStatus : qint32 {
			VERIFIED = 1,
			PREPARED = 2,
			INITIALIZED = 4,
			ERROR = 8
		};

		enum class TypeTagKind : qint8 {
			/** ReferenceType is a class.   */
			CLASS = 1,
			/** ReferenceType is an interface.   */
			INTERFACE = 2,
			/** ReferenceType is an array.   */
			ARRAY = 3
		};

		enum class Tag : qint8 {
			/** '[' - an array object (objectID size).   */
			ARRAY = 91,
			/** 'B' - a byte value (1 byte).   */
			BYTE = 66,
			/** 'C' - a character value (2 bytes).   */
			CHAR = 67,
			/** 'L' - an object (objectID size).   */
			OBJECT = 76,
			/** 'F' - a float value (4 bytes).   */
			FLOAT = 70,
			/** 'D' - a double value (8 bytes).   */
			DOUBLE = 68,
			/** 'I' - an int value (4 bytes).   */
			INT = 73,
			/** 'J' - a long value (8 bytes).   */
			LONG = 74,
			/** 'S' - a short value (2 bytes).   */
			SHORT = 83,
			/** 'V' - a void value (no bytes).   */
			VOID = 86,
			/** 'Z' - a boolean value (1 byte).   */
			BOOLEAN = 90,
			/** 's' - a String object (objectID size).   */
			STRING = 115,
			/** 't' - a Thread object (objectID size).   */
			THREAD = 116,
			/** 'g' - a ThreadGroup object (objectID size).   */
			THREAD_GROUP = 103,
			/** 'l' - a ClassLoader object (objectID size).   */
			CLASS_LOADER = 108,
			/** 'c' - a class object object (objectID size).   */
			CLASS_OBJECT = 99
		};

		enum class StepDepth : qint32 {
			/** Step into any method calls that occur before the end of the step.   */
			INTO = 0,
			/** Step over any method calls that occur before the end of the step.   */
			OVER = 1,
			/** Step out of the current method.   */
			OUT = 2
		};

		enum class StepSize : qint32 {
			/** Step by the minimum possible amount (often a bytecode instruction).   */
			MIN = 0,
			/** Step to the next source line unless there is no line number information
			 * in which case a MIN step is done instead.  */
			LINE = 1
		};

		enum class SuspendPolicy : qint8 {
			/** Suspend no threads when this event is encountered.   */
			NONE = 0,
			/** Suspend the event thread when this event is encountered.   */
			EVENT_THREAD = 1,
			/** Suspend all threads when this event is encountered.  */
			ALL = 2
		};

		/**
		 * The invoke options are a combination of zero or more of the following bit flags:
		 */
		enum class InvokeOptions : qint32 {
			/** otherwise, all threads started.   */
			INVOKE_SINGLE_THREADED = 0x01,
			/** otherwise, normal virtual invoke (instance methods only)   */
			INVOKE_NONVIRTUAL = 0x02
		};
};

/**
 * A hash function for all our enum classes. It just calls the corresponding qHash function for the underlying type.
 */
template <class Enum>
typename std::enable_if<std::is_enum<Enum>::value, uint>::type
qHash(const Enum& val) {
	return ::qHash(static_cast<typename std::underlying_type<Enum>::type>(val)); }

} /* namespace OODebug */
