#ifndef __D_ARRAY_H__
#define __D_ARRAY_H__

#include <stdlib.h>
#include <string.h>

template <class T>
class ulDArray{
public:
                                ulDArray(
                                    const int
				);
                                ~ulDArray(void);
    T&                          operator[](
                                    const int
				);
    T                           operator[](
                                    const int
				) const;
    int                         Size(void) const;
    void                        Append(
                                    const T&
                                );
    bool                        Remove(
                                    const T&
                                );
    bool                        Find(
                                    int&                    index,
                                    const T&                RHS
                                );
    void                        Clear(void);
    T*                          Array(void);
protected:
    int                         m_Size;
    int                         m_ActualSize;
    int                         m_Inc;
    T*                          m_Arr;
private:
    // hidden copy constructor
                                ulDArray(const ulDArray&);
    // hidden assignment operator
    const ulDArray&               operator= (const ulDArray&);
};

template <class T>
inline
ulDArray<T>::ulDArray(
    const int               nInc
)
    : m_Size(0),
      m_ActualSize(0),
      m_Inc(nInc),
      m_Arr(NULL)
{
}

template <class T>
inline
ulDArray<T>::~ulDArray(void)
{
    if (m_Arr)
        free(m_Arr);
}

template <class T>
T&
ulDArray<T>::operator[](
    const int               nElement
)
{
    if ((m_Arr== NULL) ||
        (m_Size<= nElement) ||
        (nElement< 0)){
	return *((T*)NULL);
    }
    return m_Arr[nElement];
}

template <class T>
T
ulDArray<T>::operator[](
    const int               nElement
) const
{
    if ((m_Arr== NULL) ||
        (m_Size<= nElement) ||
        (nElement< 0)){
	return *((T*)NULL);
    }
    return m_Arr[nElement];
}

template <class T>
int
ulDArray<T>::Size(void) const
{
    return m_Size;
}

template <class T>
void
ulDArray<T>::Append(
    const T&                RHS
)
{
    if (m_Size++>= m_ActualSize){
        m_ActualSize+= m_Inc;
        T*          temp= (T*)malloc(m_ActualSize*sizeof(T));
        if (m_Arr){
            memcpy((void*)temp, (const void*)m_Arr, (m_Size- 1)* sizeof(T));
            free(m_Arr);
        }
        m_Arr= temp;
    }
    m_Arr[m_Size- 1]= RHS;
}

template <class T>
bool
ulDArray<T>::Remove(
    const T&                RHS)
{
    // the memory after being moving will NOT be cleared to 0
    int                     index;
    if (Find(index, RHS)== false)
        return false;
    if (index!= (m_Size-1)){
        // is NOT the last one to be removed
	memmove((void*)&m_Arr[index], (void*)&m_Arr[index+ 1], (m_Size- index- 1)*sizeof(T));
    }
    m_Size--;
    return true;
}

template <class T>
void
ulDArray<T>::Clear(void)
{
    if (m_Arr){
        free(m_Arr);
        m_Arr= NULL;
    }
    m_Size= 0;
    m_ActualSize = 0;
}

template <class T>
T*
ulDArray<T>::Array(void)
{
    return m_Arr;
}

template <class T>
bool
ulDArray<T>::Find(
    int&                    index,
    const T&                RHS)
{
    int                     i;
    for (i= 0; i< m_Size; i++){
        if (m_Arr[i]== RHS){
            index= i;
            return true;
        }
    }
    return false;
}

#endif
