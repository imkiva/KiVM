//
// Created by kiva on 2019-06-22.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/cfg/edge.h>
#include <kivm/oop/method.h>
#include <set>

namespace kivm {
    namespace instr {
        class IBlock {
        public:
            /**
             * Add an edge with type {@link EdgeType#CAUGHT_EXCEPTION} to this node.
             *
             * @param handler exception handler
             * @param exceptionType type of the caught exception
             *
             * @see EdgeType#CAUGHT_EXCEPTION
             */
            virtual void addExceptionHandler(IBlock *block, String exceptionType) = 0;

            /**
             * Marks an instruction as being part of this node.
             *
             * @param num
             * @see #getInstructionNums()
             */
            virtual void addInstructionNum(int num) = 0;

            /**
             * Adds all instruction indices from another block to this one.
             * @param other
             * @see #getInstructionNums()
             */
            virtual void addInstructionNums(IBlock *other) = 0;

            /**
             * Adds a predecessor edge with custom type and meta-data.
             *
             * @param block
             * @param edgeType
             * @param metaData meta-data, may be <code>null</code>
             */
            virtual void addPredecessor(IBlock *block, EdgeType type, Object metaData) = 0;

            /**
             * Adds a predecessor edge with type {@link EdgeType#REGULAR}.
             *
             * @param block
             */
            virtual void addRegularPredecessor(IBlock *block) = 0;

            /**
             * Adds a successor edge with type {@link EdgeType#REGULAR}.
             *
             * @param block
             */
            virtual void addRegularSuccessor(IBlock *block) = 0;

            /**
             * Adds a successor edge with custom type and meta-data.
             *
             * @param block
             * @param edgeType
             * @param metaData meta-data, may be <code>null</code>
             */
            virtual void addSuccessor(IBlock *block, EdgeType type, Object metaData) = 0;

            /**
             * Called when a block has been replaced with another.
             *
             * <p>This method is responsible for replacing all internal references
             * to <code>oldBlock</code> with <code>newBlock</code>.</p>
             *
             * @param oldBlock
             * @param newBlock
             */
            virtual void onBlockReplaced(IBlock *oldBlock, IBlock *newBlock) = 0;

            virtual bool containsInstructionNum(int num) = 0;

            /**
             * Disassembles this node.
             *
             * @param method the method this node was generated from
             * @param includeVirtual whether to also 'disassemble' virtual (ASM-generated) instructions.
             * @param printInsnIndices whether to prefix each line with the ASM instruction index
             * @return
             */
            virtual String disassemble(Method *method, bool includeVirtual, bool printInstrIndices) = 0;

            /**
             * Returns the number of actual bytecode instructions for this node.
             * @param method
             * @return
             */
            virtual int getByteCodeInstructionCount(Method *method) = 0;

            /**
             * Returns all edges of this node.
             *
             * @return
             */
            virtual std::set<Edge *> getEdges() = 0;

            /**
             * Returns the index of the first bytecode instruction in this block.
             * -1 is returned if this block is virtual or contains no instructions.
             *
             * @return
             * @see #isVirtual(Method *)
             */
            virtual int getFirstByteCodeInstructionNum(Method *method) = 0;

            /**
             * Returns the index of the first ASM instruction in this block.
             * -1 is returned if this block contains no instructions.
             *
             * <p>Note that the returned instruction might be 'virtual' (ASM-generated with
             * no equivalent in the .class file).</p>
             *
             * @return
             */
            virtual int getFirstInstructionNum() = 0;

            /**
             * Returns this block's ID.
             *
             * @return ID , may be empty if the block is not yet part of a {@link ControlFlowGraph}.
             */
            virtual String getId() = 0;

            /**
             * Returns the index of the INVOKESPECIAL instruction in this 
             * node that invokes the super-classes constructor.
             * 
             * @param method the method this node was generated from
             * @return index or -1 if this block does not invoke a super-constructor
             *         or the passed method is no constructor method
             */
            virtual int getIndexOfSuperConstructorCall(Method *method) = 0;

            /**
             * Returns the instruction indices covered by this block.
             * 
             * @return
             */
            virtual std::set<int> getInstructionNums() = 0;

            /**
             * Returns the regular ({@link EdgeType#REGULAR}) predecessor of this node.
             * nullptr is returned if this node has more than one regular predecessor
             * or no (regular) predecessors.
             *  
             * @return
             */
            virtual IBlock *getRegularPredecessor() = 0;

            /**
             * Returns the number of predecessors of this block with {@link EdgeType#REGULAR}.
             * 
             * @return
             * @see #getRegularPredecessors()
             * @see #getRegularPredecessor()
             */
            virtual int getRegularPredecessorCount() = 0;

            /**
             * Returns all predecessors of this block with {@link EdgeType#REGULAR}.
             * @return
             */
            virtual std::set<IBlock *> getRegularPredecessors() = 0;

            /**
             * Returns the regular ({@link EdgeType#REGULAR}) successor of this node.
             * nullptr is returned if this node has more than one regular predecessor
             * or no (regular) predecessors.
             *  
             * @return
             */
            virtual IBlock *getRegularSuccessor() = 0;

            /**
             * Returns the number of successors of this block with {@link EdgeType#REGULAR}.
             * 
             * @return
             */
            virtual int getRegularSuccessorCount() = 0;

            /**
             * Returns all successors of this block with {@link EdgeType#REGULAR}.
             * @return
             */
            virtual std::set<IBlock *> getRegularSuccessors() = 0;

            /**
             * Check whether this node has a predecessor with {@link EdgeType#REGULAR}.
             * 
             * @return
             * @see #getRegularSuccessor()
             * @see #getRegularSuccessors()
             */
            virtual bool hasRegularPredecessor() = 0;

            /**
             * Check whether this node has a successor with {@link EdgeType#REGULAR}.
             * 
             * @return
             * @see #getRegularSuccessor()
             * @see #getRegularSuccessors()
             */
            virtual bool hasRegularSuccessor() = 0;

            /**
             * Check whether this is a 'virtual' node.
             * 
             * <p>A virtual block refers only to ASM-generated nodes that
             * have no equivalent in a .class file.</p>
             * 
             * @param method
             * @return
             */
            virtual bool isVirtual(Method *method) = 0;

            /**
             * Removes a successor with @link EdgeType#REGULAR}.
             * @param block
             * @return <code>true</code> if the edge could be removed
             */
            virtual bool removeRegularSuccessor(IBlock *block) = 0;

            /**
             * Sets the ID of this block.
             * 
             * @param id ID , not empty
             * @see #getId()
             */
            virtual void setId(String id) = 0;

            /**
             * Returns a simple string representation of this block.
             * @return
             */
            virtual String toSimpleString() = 0;

            /**
             * Returns a string representation of this block.
             * @return
             */
            virtual String toString() = 0;
        };
    }
}
