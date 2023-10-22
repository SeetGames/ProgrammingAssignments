/*!*****************************************************************************
 \file polynomial.tpp
 \author Seetoh Wei Tung
 \par DP email: seetoh.w@digipen.edu
 \par Course:  Modern C++ Design Patterns
 \par Section: B
 \par Take Home Programming Quiz 5
 \date 22-10-2023
 \brief
    Implements Polynomial class template that represents a polynomial. It takes 
    a template type parameter specifying the type of coefficients and a
    non-template type parameter specifying the polynomial's degree.
*******************************************************************************/

namespace HLP3
{
  /*!*****************************************************************************
   * @brief Default constructor. Creates a zero polynomial
   *******************************************************************************/
  template <typename CoefficientType, int Degree>
  Polynomial<CoefficientType, Degree>::Polynomial()
  {
    // Use for range loop to initialize all the coefficients to 0
    for (auto &element : m_Coefficients)
    {
      element = 0;
    }
  }

  /*!*****************************************************************************
   * @brief Single argument conversion constructor. Creates a polynomial from
   another polynomial. Note that a polynomial of degree m cannot be constructed
   with another polynomial of degree n. However, the types of coefficients for
   polynomials could be different.
   example: HLP3::Polynomial<int,2> p2( pf_1 ); // convert
   *
   * @param _copy
  *******************************************************************************/
  template <typename CoefficientType, int Degree>
  template <typename CopyCoefficientType>
  Polynomial<CoefficientType, Degree>::Polynomial(Polynomial<CopyCoefficientType, Degree> const &_copy)
  {
    for (int i = 0; i <= Degree; i++)
    {
      m_Coefficients[i] = _copy[i];
    }
  }

  /*!*****************************************************************************
   * @brief Copy assignment operator. Assigns a polynomial to another polynomial.
   Note that both polynomial must have the same degree. However, the types of
   coefficients for polynomials could be different.
   *
   * @param _copy
   * @return Polynomial<T, N>&
  *******************************************************************************/
  template <typename CoefficientType, int Degree>
  template <typename RhsCoefficientType>
  Polynomial<CoefficientType, Degree> &Polynomial<CoefficientType, Degree>::operator=(Polynomial<RhsCoefficientType, Degree> const &_rhs)
  {
    for (int i = 0; i <= Degree; i++)
    {
      m_Coefficients[i] = _rhs[i];
    }
    return *this;
  }

  /*!*****************************************************************************
   * @brief Modifiable operator[]. Returns a reference to the coefficient of the
      term with the given exponent.
   *
   * @param _exponent
   * @return CoefficientType&
   *******************************************************************************/
  template <typename CoefficientType, int Degree>
  CoefficientType &Polynomial<CoefficientType, Degree>::operator[](int _exponent)
  {
    return m_Coefficients[_exponent];
  }

  /*!*****************************************************************************
   * @brief Non-modifiable operator[]. Returns a reference to the coefficient of
   the term with the given exponent.
   *
   * @param _exponent
   * @return CoefficientType const&
   *******************************************************************************/
  template <typename CoefficientType, int Degree>
  CoefficientType const &Polynomial<CoefficientType, Degree>::operator[](int _exponent) const
  {
    return m_Coefficients[_exponent];
  }
  
  /*!*****************************************************************************
   * @brief Given polynomials p(x) and q(x), return p(x)*q(x). Note that this
      function must only allow polynomials with similar coefficient types to be
      multiplied.
   *
   * @param _rhs
   * @return Polynomial<CoefficientType, Degree>
   *******************************************************************************/
  template <typename CoefficientType, int Degree>
  template <int RhsDegree>
  Polynomial<CoefficientType, Degree + RhsDegree> Polynomial<CoefficientType, Degree>::operator*(Polynomial<CoefficientType, RhsDegree> const &_rhs) const
  {
    Polynomial<CoefficientType, Degree + RhsDegree> result{};
    for (int lhsDegree = 0; lhsDegree <= Degree; lhsDegree++)
    {
      for (int rhsDegree = 0; rhsDegree <= RhsDegree; rhsDegree++)
      {
        result[lhsDegree + rhsDegree] += m_Coefficients[lhsDegree] * _rhs[rhsDegree];
      }
    }
    return result;
  }

  /*!*****************************************************************************
   * @brief Evaluage polynomial at x = a. Evaluate polynomial p(x) using 
   argument x and return the value.
   * 
   * @param _x 
   * @return CoefficientType 
  *******************************************************************************/
  template <typename CoefficientType, int Degree>
  CoefficientType Polynomial<CoefficientType, Degree>::operator()(CoefficientType const &_x) const
  {
    CoefficientType result = 0;
    CoefficientType xRaisedToExponent = 1;
    for (int exponent = 0; exponent <= Degree; exponent++)
    {
      result += m_Coefficients[exponent] * xRaisedToExponent;
      xRaisedToExponent *= _x;
    }
    return result;
  }
  
  // DON'T CHANGE/EDIT THE FOLLOWING DEFINITION:
  template <typename T, int N>
  std::ostream &operator<<(std::ostream &out, Polynomial<T, N> const &pol)
  {
    out << pol[0] << " ";
    for (int i = 1; i <= N; ++i)
    {
      if (pol[i] != 0)
      { // skip terms with zero coefficients
        if (pol[i] > 0)
        {
          out << "+";
        }
        if (pol[i] == 1)
        {
        }
        else if (pol[i] == -1)
        {
          out << "-";
        }
        else
        {
          out << pol[i] << "*";
        }
        out << "x^" << i << " ";
      }
    }
    return out;
  }

} // end namespace HLP3
