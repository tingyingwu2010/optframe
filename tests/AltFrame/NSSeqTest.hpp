#pragma once

#include "NSSeqBase.hpp"

#ifdef CORO // enable coroutines
#include <OptFCore/coro/Generator.hpp>
#endif

#ifdef USECPPCORO
#include <AltFrame/cppcoro/generator.hpp>
#endif

// my test
// vector<int>
// swap idea
// =================

class NSSeqTestStateless : public NSSeq<std::vector<int>, MoveFPtr<std::vector<int>>, SNSIteratorFuncPtrCopy<std::vector<int>, MoveFPtr<std::vector<int>>>>
{
public:
   using MyMove = MoveFPtr<std::vector<int>>;

public:
   int nTSP{ -1 };

   static int st_nTSP;

   NSSeqTestStateless(int nTSP)
     : nTSP{ nTSP }
#ifdef CORO // enable coroutines
     , gen{make_gen()}
#endif
   {
      // keep static problem data
      st_nTSP = nTSP;
      commonState = std::make_pair(0,1);
   }

   // store move methods on NSSeq

   /*
   std::function<void(std::vector<int>&)> fApplyDo
   {
      [](std::vector<int>& v) -> void {
            int& i = commonState.first;
            int& j = commonState.second;
            // swap
            int aux = v[i];
            v[i] = v[j];
            v[j] = aux;
         }
   };
   */

   static void fApplyDo(std::vector<int>& v)
   {
      // non-capture!
      //[](std::vector<int>& v) -> void {
      int& i = commonState.first;
      int& j = commonState.second;
      // swap
      int aux = v[i];
      v[i] = v[j];
      v[j] = aux;
      //   }
   };

   //std::function<void(X&)> fApplyUndo; // TODO: receive this correctly.. now only copy for 'undo'
   //
   //std::function<MoveByContextRef<X>(State&)> fGenMoves;
   //
   static std::pair<int, int> commonState;

   /*
   // must find solution to this!
   virtual MoveFuncPtrCopy<std::vector<int>> getStateMove () {
      void(*ptr)(std::vector<int>&) = this->fApplyDo;
      return MoveFuncPtrCopy<std::vector<int>> (
         ptr
      );  // automatic implementation
   }
*/

   virtual uptr<NSIterator<std::vector<int>, MyMove>> getIterator(const std::vector<int>&);

   virtual SNSIteratorFuncPtrCopy<std::vector<int>, MyMove> getIteratorCopy(const std::vector<int>&)
   {
      return SNSIteratorFuncPtrCopy<std::vector<int>, MyMove>{
         fFirst,
         fNext,
         fIsDone,
         fCurrent
      };
   }

#ifdef CORO // enable coroutines
   // ============ COROUTINE APPROACH ============

   // for coroutines
   //op<optframe::Generator<MyMove>> op_gen;
   //
   //op<optframe::Generator<std::pair<int,int>>> op_gen;
   optframe::Generator<std::pair<int,int>> gen;
   //
   bool done;
   //

   //optframe::Generator<MyMove> make_gen(const std::vector<int>& v)
   optframe::Generator<std::pair<int,int>> make_gen()
   {
      //void (*ptr)(std::vector<int>&) = fApplyDo;
      //
      for (int i = 0; i < nTSP - 1; i++)
         for (int j = i + 1; j < nTSP; j++)
            //co_yield MoveFPtr<std::vector<int>>{ ptr };
            co_yield std::make_pair(i,j);
   }

   bool coroFirst(const std::vector<int>& v)
   {
      //std::cout << "FIRST!" << std::endl;
      gen = make_gen();
      done = !gen.next(); // advance and update bool
      return done;
   }

   bool coroNext()
   {
      done = !gen.next(); // advance and update bool
      return done;
   }

   bool coroIsDone()
   {
      return done;
   }

   ///static constexpr MoveFPtr<std::vector<int>> mv_coro{ fApplyDo };
      

   op<MyMove> coroCurrent()
   {
      //void (*ptr)(std::vector<int>&) = fApplyDo;
      //MoveFPtr<std::vector<int>> mv{ ptr };
      MoveFPtr<std::vector<int>> mv{ fApplyDo };
      commonState = gen.getValue();
      //return op<MyMove>{ std::move(mv) };
      return op<MyMove>{ mv };

      //return op<MyMove>(
      //  op_gen->getValue()); // automatic implementation
   }
#endif

   // ==================== REF STATIC ITER ================

   static std::function<void(std::vector<int>&)> funcApply;
   
   static op<MoveFRef<std::vector<int>>> fCurrentRef()
   {
      return op<MoveFRef<std::vector<int>>>(
         //
        MoveFRef<std::vector<int>>(funcApply)
        // 
        ); 
   }

   // tl reference
   static op<MoveFTL<std::vector<int>>> fCurrentRefTL()
   {
      return op<MoveFTL<std::vector<int>>>(
         //
        MoveFTL<std::vector<int>>(funcApply)
        // 
        ); 
   }

   static op<MoveFCopy<std::vector<int>>> fCurrentMoveFCopy()
   {
      return op<MoveFCopy<std::vector<int>>>(
         //
        MoveFCopy<std::vector<int>>(funcApply)
        // 
        ); 
   }


class MoveInternal final
{
public:
  
  void apply(std::vector<int>& v)
  {
      int& i = commonState.first;
      int& j = commonState.second;
      // swap
      int aux = v[i];
      v[i] = v[j];
      v[j] = aux;
  }

  void undo(std::vector<int>& v)
  {
     return apply(v);
  }

   MoveInternal* operator->()
   {
      return this;
   }

   void print()
   {
      std::cout << "MOVE!" << std::endl;
   }
};

   static op<MoveInternal> fCurrentMoveInternal()
   {
      //int x = 100'000'000;
      return op<MoveInternal>(
         //
        MoveInternal{}
        // 
        ); 
   }

   // ==================== STATIC ITERATOR ====================

   static bool fFirst(const std::vector<int>&)
   {
      commonState.first = 0;
      commonState.second = 1;
      return true;
   }

   static bool fNext()
   {
      commonState.second++;
      if (commonState.second >= (st_nTSP - 1)) {
         commonState.first++;
         commonState.second = commonState.first + 1;
      }
      return commonState.first < st_nTSP;
   }

   static bool fIsDone()
   {
      return commonState.first >= (st_nTSP - 1);
   }

   static op<MyMove> fCurrent()
   {
      //void (*ptr)(std::vector<int>&) = fApplyDo;
      //
      return op<MyMove>(
        //MoveFPtr<std::vector<int>>(ptr)
        MoveFPtr<std::vector<int>>(fApplyDo)
        // automatic implementation
        ); 
   }

   static op<MoveFPtr<std::vector<int>>> staticMoveFPtr;

   static const op<MoveFPtr<std::vector<int>>>& ref_fCurrent()
   {
      return staticMoveFPtr;
   }

   // ==================== EMBEDDED ITERATOR

   // returns false if finished
   virtual bool first(const std::vector<int>&)
   {
      commonState.first = 0;
      commonState.second = 1;
      /* whatever */
      return true;
   } //= 0;

   // returns false if finished
   virtual bool next()
   {
      commonState.second++;
      if (commonState.second >= (nTSP - 1)) {
         commonState.first++;
         commonState.second = commonState.first + 1;
      }
      return commonState.first < nTSP;
   } // = 0;

   // returns true if finished (TODO: we may remove this)
   virtual bool isDone() const
   {
      return commonState.first >= (nTSP - 1);
      /* whatever */
   } // = 0;

   // returns current move, or nothing (if doesn't exist)
   //virtual op< MoveFuncPtrCopy<std::vector<int>> > current(){ /* whatever */ } // = 0;
   virtual op<MoveFPtr<std::vector<int>>> current()
   {
      //void (*ptr)(std::vector<int>&) = this->fApplyDo;
      //
      return op<MoveFPtr<std::vector<int>>>(
        //MoveFPtr<std::vector<int>>(ptr)
        MoveFPtr<std::vector<int>>(fApplyDo)
          // automatic implementation
          ); 
   }
};

int NSSeqTestStateless::st_nTSP = 0;

std::pair<int, int> NSSeqTestStateless::commonState = std::pair<int, int>{0,1};

std::function<void(std::vector<int>&)> NSSeqTestStateless::funcApply = 
               [](std::vector<int>& v) -> void {
                        int& i = commonState.first;
                        int& j = commonState.second;
                        // swap
                        int aux = v[i];
                        v[i] = v[j];
                        v[j] = aux;
                     };

op<MoveFPtr<std::vector<int>>> NSSeqTestStateless::staticMoveFPtr = 
      MoveFPtr<std::vector<int>>(NSSeqTestStateless::fApplyDo);
          

class MyIter : public NSIterator<std::vector<int>, NSSeqTestStateless::MyMove>
{
public:
   int nTSP{ -1 };

   MyIter(int _nTSP)
     : nTSP{ _nTSP }
   {
   }

   // returns false if finished
   virtual bool first(const std::vector<int>&)
   {
      NSSeqTestStateless::commonState.first = 0;
      NSSeqTestStateless::commonState.second = 1;
      /* whatever */
      return true;
   } //= 0;

   // returns false if finished
   virtual bool next()
   {
      NSSeqTestStateless::commonState.second++;
      if (NSSeqTestStateless::commonState.second >= (nTSP - 1)) {
         NSSeqTestStateless::commonState.first++;
         NSSeqTestStateless::commonState.second = NSSeqTestStateless::commonState.first + 1;
      }
      return NSSeqTestStateless::commonState.first < nTSP;
   } // = 0;

   // returns true if finished (TODO: we may remove this)
   virtual bool isDone() const
   {
      return NSSeqTestStateless::commonState.first >= (nTSP - 1);
      /* whatever */
   } // = 0;

   // returns current move, or nothing (if doesn't exist)
   //virtual op< MoveFuncPtrCopy<std::vector<int>> > current(){ /* whatever */ } // = 0;
   virtual op<MoveFPtr<std::vector<int>>> current()
   {
      //void (*ptr)(std::vector<int>&) = NSSeqTestStateless::fApplyDo;
      //
      return op<MoveFPtr<std::vector<int>>>(
         //MoveFPtr<std::vector<int>>(ptr)
         MoveFPtr<std::vector<int>>(NSSeqTestStateless::fApplyDo)
         // automatic implementation
          ); 
   }
};

uptr<NSIterator<std::vector<int>, NSSeqTestStateless::MyMove>>
NSSeqTestStateless::getIterator(const std::vector<int>&)
{
   return uptr<NSIterator<std::vector<int>, NSSeqTestStateless::MyMove>>(new MyIter{ this->nTSP });
}