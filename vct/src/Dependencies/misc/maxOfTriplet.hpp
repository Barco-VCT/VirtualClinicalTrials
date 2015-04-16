#ifndef MAXOFTRIPLET_HPP
#define MAXOFTRIPLET_HPP

/**
 * @param a the first input
 * @param b the second input input
 * @param c the third input
 * @return the largest value between the three input parameters
 */
template<typename T>
inline T maxOfTriplet(T a, T b, T c)
{
    if(a>b)
        if(a>c)
            return a;
    else
        return c;
    else if(b>c)
        return b;
    else
        return c;
}

#endif // MAXOFTRIPLET_HPP
