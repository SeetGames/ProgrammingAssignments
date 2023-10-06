/*!*****************************************************************************
 \file matrix-proxy.cpp
 \author Seetoh Wei Tung
 \par DP email: seetoh.w\@digipen.edu
 \par Course: HLP3
 \par Section: B
 \par Programming Assignment #4
 \date 6-10-2023
 \brief
    Defines a C++ style API for matrices using move semantics and proxy design pattern
*******************************************************************************/

#ifndef MATRIX_PROXY_HPP
#define MATRIX_PROXY_HPP

#include <iostream>
#include <stdexcept>
#include <initializer_list>

namespace HLP3
{
    template <typename T>
    class Matrix
    {

    public:
        using value_type = T;
        using size_type = std::size_t;
        using pointer = T**;

        class Proxy
        {
        public:
            /*!*****************************************************************************
             \brief Construct a new Proxy object
             
             \param[in] _matrix 
             \param[in] _row 
            *******************************************************************************/
            Proxy(Matrix &_matrix, size_type _row) 
            : m_Matrix(_matrix), 
            m_Row(_row) 
            {}

            /*!*****************************************************************************
             \brief Subscript operator
             
             \param[in] _column 
             \return T& 
            *******************************************************************************/
            T &operator[](size_type _column)
            {
                return m_Matrix.m_Data[m_Row][_column];
            }

        private:
            Matrix &m_Matrix;
            size_type m_Row;
        };

        class ConstProxy
        {
        public:
            /*!*****************************************************************************
             \brief Construct a new Const Proxy object
             
             \param[in] _matrix 
             \param[in] _row 
            *******************************************************************************/
            ConstProxy(const Matrix &_matrix, size_type _row) 
            : m_Matrix(_matrix), 
            m_Row(_row) 
            {}

            /*!*****************************************************************************
             \brief Subscript operator
             
             \param[in] _columns 
             \return const T& 
            *******************************************************************************/
            const T &operator[](size_type _columns) const
            {
                return m_Matrix.m_Data[m_Row][_columns];
            }

        private:
            const Matrix &m_Matrix;
            size_type m_Row;
        };

        /*!*****************************************************************************
         \brief Construct a new Matrix object of _rows x _columns
         
         \param[in] _rows 
         \param[in] _columns 
        *******************************************************************************/
        Matrix(size_type _rows, size_type _columns) 
        : m_Rows(_rows), 
        m_Columns(_columns)
        {
            m_Data = new T *[m_Rows];
            for (size_type i = 0; i < m_Rows; ++i)
            {
                m_Data[i] = new T[m_Columns]();
            }
        }

        /*!*****************************************************************************
         \brief Copy constructor of a Matrix object. Performs a deep copy of its data.
         
         \param[in] _matrixCopy 
        *******************************************************************************/
        Matrix(const Matrix &_matrixCopy) 
        : m_Rows(_matrixCopy.m_Rows), 
        m_Columns(_matrixCopy.m_Columns)
        {
            m_Data = new T *[m_Rows];
            for (size_type row = 0; row < m_Rows; ++row)
            {
                m_Data[row] = new T[m_Columns];
                for (size_type column = 0; column < m_Columns; ++column)
                {
                    m_Data[row][column] = _matrixCopy.m_Data[row][column];
                }
            }
        }

        /*!*****************************************************************************
         \brief Move constructor of a Matrix object.
         
         \param[in] _movedMatrix 
        *******************************************************************************/
        Matrix(Matrix &&_movedMatrix) noexcept 
        : m_Rows(std::exchange(_movedMatrix.m_Rows,0)), 
        m_Columns(std::exchange(_movedMatrix.m_Columns,0)),
        m_Data(std::exchange(_movedMatrix.m_Data,nullptr))
        {
        }

        /*!*****************************************************************************
         \brief Construct a new Matrix object with an initializer list
         
         \param[in] _matrixList 
        *******************************************************************************/
        Matrix(std::initializer_list<std::initializer_list<value_type>> _matrixList)
        {
            m_Rows = _matrixList.size();
            //Number of items in the first list of the list
            m_Columns = _matrixList.begin()->size();

            //Check if the list doesn't have rows of equal size
            for (const auto &inner_list : _matrixList)
            {
                if (inner_list.size() != m_Columns)
                {
                    throw std::runtime_error("bad initializer list");
                }
            }
            //Construct the rows
            m_Data = new T *[m_Rows];
            size_type row = 0;
            for (const auto &inner_list : _matrixList)
            {
                m_Data[row] = new T[m_Columns];
                size_type column = 0;
                for (const auto &listValue : inner_list)
                {
                    m_Data[row][column] = listValue;
                    ++column;
                }
                ++row;
            }
        }

        /*!*****************************************************************************
         \brief Destroy the Matrix objectdestroys the matrix by explicitly returning
         it to free storage
         
        *******************************************************************************/
        ~Matrix() noexcept
        {
            for (size_type row = 0; row < m_Rows; ++row)
            {
                delete[] m_Data[row];
            }
            delete[] m_Data;
        }

        /*!*****************************************************************************
         \brief Assignment operator overload. Replaces the matrix with a deep copy of
         _matrixCopy
         
         \param[in] _matrixCopy 
         \return Matrix& 
        *******************************************************************************/
        Matrix &operator=(const Matrix &_matrixCopy)
        {
            if (this == &_matrixCopy)
                return *this;

            //Older matrix might be smaller or bigger than the copy.
            for (size_type i = 0; i < m_Rows; ++i)
            {
                delete[] m_Data[i];
            }
            delete[] m_Data;

            m_Rows = _matrixCopy.m_Rows;
            m_Columns = _matrixCopy.m_Columns;

            m_Data = new T *[m_Rows];
            for (size_type row = 0; row < m_Rows; ++row)
            {
                m_Data[row] = new T[m_Columns];
                for (size_type column = 0; column < m_Columns; ++column)
                {
                    m_Data[row][column] = _matrixCopy.m_Data[row][column];
                }
            }

            return *this;
        }

        /*!*****************************************************************************
         \brief Move assignment operator.
         
         \param[in] _movedMatrix 
         \return Matrix& 
        *******************************************************************************/
        Matrix &operator=(Matrix &&_movedMatrix) noexcept
        {
            if (this == &_movedMatrix)
                return *this;

            for (size_type row = 0; row < m_Rows; ++row)
            {
                delete[] m_Data[row];
            }
            delete[] m_Data;
            
            m_Rows = std::exchange(_movedMatrix.m_Rows,0);
            m_Columns = std::exchange(_movedMatrix.m_Columns,0);
            m_Data = std::exchange(_movedMatrix.m_Data,nullptr);

            return *this;
        }
        
        /*!*****************************************************************************
         \brief Get the number of rows
         
         \return size_type 
        *******************************************************************************/
        size_type get_rows() const noexcept
        {
            return m_Rows;
        }

        /*!*****************************************************************************
         \brief Get the number of columns
         
         \return size_type 
        *******************************************************************************/
        size_type get_cols() const noexcept
        {
            return m_Columns;
        }

        /*!*****************************************************************************
         \brief Return index of _row
         
         \param[in] _row 
         \return Proxy 
        *******************************************************************************/
        Proxy operator[](size_type _row)
        {
            return Proxy(*this, _row);
        }

        /*!*****************************************************************************
         \brief Return index of _row
         
         \param[in] _row 
         \return ConstProxy 
        *******************************************************************************/
        ConstProxy operator[](size_type _row) const
        {
            return ConstProxy(*this, _row);
        }

    private:
        size_type m_Rows;
        size_type m_Columns;
        pointer m_Data;
    };

    /*!*****************************************************************************
     \brief Returns true if the matrices compared are exactly equivalent.
     
     \tparam[in] T 
     \param[in] _lhs 
     \param[in] _rhs 
     \return true 
     \return false 
    *******************************************************************************/
    template <typename T>
    bool operator==(const Matrix<T> &_lhs, const Matrix<T> &_rhs)
    {
        if (_lhs.get_rows() != _rhs.get_rows() || _lhs.get_cols() != _rhs.get_cols())
            return false;

        for (size_t row = 0; row < _lhs.get_rows(); ++row)
        {
            for (size_t column = 0; column < _lhs.get_cols(); ++column)
            {
                if (_lhs[row][column] != _rhs[row][column])
                    return false;
            }
        }
        return true;
    }

    /*!*****************************************************************************
     \brief Returns true if the matrices compared are not equivalent.
     
     \tparam[in] T 
     \param[in] _lhs 
     \param[in] _rhs 
     \return true 
     \return false 
    *******************************************************************************/
    template <typename T>
    bool operator!=(const Matrix<T> &_lhs, const Matrix<T> &_rhs)
    {
        return !(_lhs == _rhs);
    }

    /*!*****************************************************************************
     \brief Returns matrix with sum _lhs + _rhs
     
     \tparam[in] T 
     \param[in] _lhs 
     \param[in] _rhs 
     \return Matrix<T> 
    *******************************************************************************/
    template <typename T>
    Matrix<T> operator+(const Matrix<T> &_lhs, const Matrix<T> &_rhs)
    {
        if (_lhs.get_rows() != _rhs.get_rows() || _lhs.get_cols() != _rhs.get_cols())
        {
            throw std::runtime_error("operands for matrix addition must have same dimensions");
        }

        Matrix<T> resultantMatrix(_lhs.get_rows(), _lhs.get_cols());
        for (size_t row = 0; row < _lhs.get_rows(); ++row)
        {
            for (size_t column = 0; column < _lhs.get_cols(); ++column)
            {
                resultantMatrix[row][column] = _lhs[row][column] + _rhs[row][column];
            }
        }

        return resultantMatrix;
    }

    /*!*****************************************************************************
     \brief Returns matrix with difference _lhs - _rhs
     
     \tparam[in] T 
     \param[in] _lhs 
     \param[in] _rhs 
     \return Matrix<T> 
    *******************************************************************************/
    template <typename T>
    Matrix<T> operator-(const Matrix<T> &_lhs, const Matrix<T> &_rhs)
    {
        if (_lhs.get_rows() != _rhs.get_rows() || _lhs.get_cols() != _rhs.get_cols())
        {
            throw std::runtime_error("operands for matrix subtraction must have same dimensions");
        }

        Matrix<T> resultantMatrix(_lhs.get_rows(), _lhs.get_cols());
        for (size_t row = 0; row < _lhs.get_rows(); ++row)
        {
            for (size_t column = 0; column < _lhs.get_cols(); ++column)
            {
                resultantMatrix[row][column] = _lhs[row][column] - _rhs[row][column];
            }
        }

        return resultantMatrix;
    }

    /*!*****************************************************************************
     \brief Returns matrix that is product _lhs x _rhs
     
     \tparam[in] T 
     \param[in] _lhs 
     \param[in] _rhs 
     \return Matrix<T> 
    *******************************************************************************/
    template <typename T>
    Matrix<T> operator*(const Matrix<T> &_lhs, const Matrix<T> &_rhs)
    {
        if (_lhs.get_cols() != _rhs.get_rows())
        {
            throw std::runtime_error("number of columns in left operand must match number of rows in right operand");
        }

        Matrix<T> resultantMatrix(_lhs.get_rows(), _rhs.get_cols());
        for (size_t row = 0; row < _lhs.get_rows(); ++row)
        {
            for (size_t column = 0; column < _rhs.get_cols(); ++column)
            {
                T sum = 0;
                for (size_t k = 0; k < _lhs.get_cols(); ++k)
                {
                    sum += _lhs[row][k] * _rhs[k][column];
                }
                resultantMatrix[row][column] = sum;
            }
        }

        return resultantMatrix;
    }

    /*!*****************************************************************************
     \brief Returns the matrix scaled by _scalar
     
     \tparam[in] T 
     \param[in] _scalar 
     \param[in] _matrix 
     \return Matrix<T> 
    *******************************************************************************/
    template <typename T>
    Matrix<T> operator*(T _scalar, const Matrix<T> &_matrix)
    {
        Matrix<T> resultantMatrix(_matrix.get_rows(), _matrix.get_cols());
        for (size_t i = 0; i < _matrix.get_rows(); ++i)
        {
            for (size_t j = 0; j < _matrix.get_cols(); ++j)
            {
                resultantMatrix[i][j] = _scalar * _matrix[i][j];
            }
        }
        return resultantMatrix;
    }

    /*!*****************************************************************************
     \brief Returns the matrix scaled by _scalar
     
     \tparam[in] T 
     \param[in] _matrix 
     \param[in] _scalar 
     \return Matrix<T> 
    *******************************************************************************/
    template <typename T>
    Matrix<T> operator*(const Matrix<T> &_matrix, T _scalar)
    {
        return _scalar * _matrix; // Leveraging the previous operator for efficiency
    }
} // end of HLP3 namespace

#endif // MATRIX_PROXY_HPP
