#ifndef UTIL_HPP
#define UTIL_HPP

#include <type_traits>
#include <typeinfo>
#include <typeindex>
#include <functional>

#ifndef FWD
#define FWD(X,Y) std::forward<X>(Y)
#endif

#ifndef SELF
#define SELF(Y) std::forward<decltype(Y)>(Y)
#endif

namespace Detail { // --> util/types.hpp

	/** @brief A tag type representing no type */
	struct undef_t;

	/** @brief A tag type representing a list delimiter */
	struct delim_t;

	/**
	 * @brief A type used to find the static dimension of a type
	 * @tparam T A statically-sized type
	 */
	template<class T> struct sized_t;

	/**
	 * @brief Reduces a variadic list of types to the sum of their sizes
	 * @tparam T1 The head of the argument types
	 * @tparam TN The remainder of the template arguments
	 */
	template<class T1, class... TN> struct sizes_t;

	/**
	 * @brief A tag type storing types with duplicates
	 * @tparam T The unordered list of types
	 */
	template<class... T> struct pack_t;

	/**
	 * @brief A tag type storing integers with duplicates
	 * @tparam I The unordered list of integers
	 */
	template<int... I> struct pack_i;

	/**
	 * @brief A tag type storing types without duplicates
	 * @tparam T The unordered list of unique types
	 */
	template<class... T> struct set_t;

	/**
	 * @brief A tag representing a node in a graph
	 * @tparam T The type corresponding to the node
	 */
	template<class T> struct node_t;

	/**
	 * @brief A tag representing an edge between two nodes in a graph
	 * @tparam U The source of the edge, or both
	 * @tparam V The destination of the edge, or both
	 */
	template<class U, class V = U> struct edge_t;

	/**
	 * @brief A tag representing a graph
	 * @tparam V The pack of types containing the vertex endpoints
	 * @tparam E The pack of integers containing the endpoint indices
	 */
	template<class V, class E> struct graph_t;
}
#include "util/types.hpp"

namespace Detail { // --> util/functional.hpp
	/**
	 * @brief A tag representing the signature of a function
	 * @tparam S A function or function object type
	 */
	template<class S>
		struct sig_t;

	/**
	 * @brief A type exposing an application function over a permutation
	 * of elements of array types
	 * @tparam SEQ The unique ID of the permutation
	 * @tparam CUR The index into the permutation
	 * @tparam TN The unordered list of array types
	 */
	template<int SEQ, int CUR, class... TN>
		struct for_seq_t;

	/**
	 * @brief A type exposing an application function over all permutations
	 * of elements of array types 
	 * @tparam SEQ The unique ID of the current permutation
	 * @tparam FN The type of the function or function object to apply
	 * @tparam TN The unordered list of array types
	 */
	template<int SEQ, class FN, class... TN>
		struct for_all_t;

	/**
	 * @brief Maps each array to another type before applying a function to
	 * each permutation of the resulting elements.
	 * @tparam SEQ The unique ID of the current sequence
	 * @tparam FN The function to apply to each permutation
	 * @tparam T1 The head of the array types
	 * @tparam TN The remainder of the array types
	 */
	template<int SEQ, class FN, class T1, class... TN>
		struct map_for_all_t;

	/**
	 * @brief Applies a function to each permutation in the diagonal of all
	 * permutations of the elements of the given statically-sized arrays
	 * (all first elements, all second elements, etc.)
	 * @tparam SEQ The unique ID of the current sequence
	 * @tparam FN The function to apply to each selected permutation
	 * @tparam N The size of each array type
	 * @tparam T1 The head of the list of array types
	 * @tparam TN The remainder of the array types
	 */
	template<int SEQ, int N, class FN, class T1, class... TN>
		struct for_zip_t;
}
#include "util/functional.hpp"

template<typename... S>
void zipper(std::function<void(S...)> fn) {}
template<typename S1, typename... SN, typename T1, typename... TN,
	template<typename> class FN = std::function>
void zipper(FN<void(S1, SN...)> fn,
		S1 && s1, SN &&... sn,
		T1 && t1, TN &&... tn) {
	fn(SELF(s1), SELF(sn)...);
	zipper(fn, SELF(t1), SELF(tn)...);
}

template<typename FN, int N, typename T1, typename... TN>
void for_zip(FN fn, T1 (&t1)[N], TN (&...tn)[N]) {
	using namespace Detail;
	for_zip_t<N, N, FN, T1, TN...>::apply(fn,
			FWD(T1(&)[N],t1), FWD(TN(&)[N],tn)...);
}

template<typename T1, typename... TN>
constexpr int SizeProduct(void) {
	return Detail::sizes_t<T1, TN..., Detail::delim_t>::SIZE;
}
template<typename T1, typename... TN>
constexpr int SizeProduct(T1 && t1, TN &&... tn) {
	return SizeProduct<T1, TN...>();
}

/** Applies fn to one element (SEQ) of the cartesian product of
 * the argument arrays. */
template<int SEQ, typename FN, typename T1, typename... TN>
void for_seq(FN fn, T1 && t1, TN &&... tn) {
	using namespace Detail;
	for_seq_t<SEQ, 1 + sizeof...(TN), FN, T1, TN...>
			::apply(fn, FWD(T1,t1), FWD(TN,tn)...);
}

/** Applies fn to each element of the cartesian product of the
 * argument arrays. */
template<typename FN, typename T1, typename... TN,
	int N = Detail::sizes_t<T1, TN..., Detail::delim_t>::SIZE>
void for_all(FN fn, T1 && t1, TN &&... tn) {
	using namespace Detail;
	for_all_t<N, FN, T1, TN...>::apply(fn,
			FWD(T1,t1), FWD(TN,tn)...);
}
template<typename FN0, typename FN1, typename... FN, typename T1,
	typename... TN, int N = Detail::sizes_t<T1, TN..., Detail::delim_t>::SIZE>
void for_all(FN0 fn0, FN1 fn1, FN... fn, T1 && t1, TN &&... tn) {
	for_all(fn0, SELF(t1), SELF(tn)...);
	for_all(SELF(fn1), SELF(fn)..., SELF(t1), SELF(tn)...);
}

/** Applies fn to each element of the cartesian product of the
 * argument arrays and stores the results sequentially in out. */
template<typename FN, typename T1, typename... TN,
	typename RET = typename Detail::sig_t<FN>::result_type,
	int N = Detail::sizes_t<T1, TN..., Detail::delim_t>()>
auto map_for_all(FN fn, RET (&out)[N],
		T1 && t1, TN &&... tn) -> decltype(out) {
	using namespace Detail;
	map_for_all_t<N, FN, T1, TN...>::apply(fn,
			FWD(RET (&)[N],out), FWD(T1,t1), FWD(TN,tn)...);
	return FWD(RET (&)[N],out);
}

namespace test {
struct test_util {
	static bool run(void) {
		using namespace Detail;
		// Base types
		typedef pack_t<> T_void;
		typedef pack_t<int> T_I;
		typedef pack_t<float> T_F;
		typedef pack_t<double> T_D;
		typedef pack_t<int, float> T_IF;
		typedef pack_t<int, float, double> T_IFD;
		T_void V0;
		T_I VI; T_F VF; T_D VD;
		T_IF VIF; T_IFD VIFD;

		// Types with duplicates
		typedef pack_t<int,int> T_II;
		typedef pack_t<float, double, float> T_FDF;
		
		
		// Concatenation types
		static_assert(std::is_same<typename
				pack_cat<T_I, float>::type, T_IF>::value, "");
		static_assert(std::is_same<typename
				pack_cat<T_I, int>::type, T_I>::value, "");

		// Union types
		typedef typename pack_merge<T_IFD, T_void>::type T_u0;
		typedef typename pack_merge<T_void, T_IFD>::type T_u1;
		typedef typename pack_merge<T_I, T_FDF>::type T_u2;
		static_assert(std::is_same<T_u0, T_IFD>::value, "");
		static_assert(std::is_same<T_u1, T_IFD>::value, "");
		static_assert(std::is_same<T_u2, T_IFD>::value, "");

		// Index of first matching type (or -1)
		static_assert(index_of(T_void {}, int {}) < 0, "");
		static_assert(index_of(T_I {}, int {}) >= 0, "");

		// Index of first matching type (or -1) for each type
		static_assert(std::is_same<
				decltype(indices_of(VIFD, VD + VI + pack_t<char>{})),
				pack_i<2, 0, -1>>::value, "");

		// Prune duplicates
		static_assert(std::is_same<decltype(prune(VIFD)), T_IFD>::value, "");
		static_assert(std::is_same<decltype(prune(T_FDF {})),
				decltype(VF+VD)>::value, "");
		
		// Or
		static_assert(std::is_same<decltype(VI+VF+VD), T_IFD>::value, "");
		static_assert(!std::is_same<decltype(VD+VF+VI), T_IFD>::value, "");
		// Not
		static_assert(std::is_same<decltype(VIF-VF), T_I>::value, "");
		static_assert(std::is_same<decltype(VIF-VF-VI), T_void>::value, "");
		// Xor
		static_assert(std::is_same<decltype(VI^VI),
				decltype(VI-VI)>::value, "");
		static_assert(std::is_same<decltype(VI^VF),
				decltype(VI+VF)>::value, "");
		// And
		static_assert(std::is_same<decltype(VI & VI), T_I>::value, "");
		static_assert(std::is_same<decltype(VIF & VI), T_I>::value, "");

		// Permutations
		static_assert(!std::is_same<decltype(VI+VF),
				decltype(VF+VI)>::value, "");
		static_assert(permutes(VI+VF, VF+VI), "");
		static_assert(!permutes(VI+VF, VI), "");

		// Infix operators/types/etc.
		infix_t<pack_t> pack_with;
		infix_t<pack_merge> merge_with;
		infix_t<std::is_same> same_as;

		auto packed1 = 1 <pack_with> 2;
		static_assert(!inner_value(packed1 <same_as> VI), "");
		static_assert(inner_value(prune(packed1) <same_as> VI), "");

		auto packed2 = 3 <pack_with> 4.0f;
		static_assert(inner_value(packed2 <same_as> VIF), "");
	
		graph_t<T_void, T_void> G_void;
		auto G_if = G_void + node_t<int>{} + node_t<float>{};
		static_assert(inner_value(G_if <same_as> G_if), "");
		return true;
	}
};

}

#endif
