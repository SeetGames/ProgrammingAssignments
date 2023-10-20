

namespace HLP3
{
  /*!*****************************************************************************
   * @brief Default constructor. Creates a zero polynomial
   *******************************************************************************/
  template <typename CoefficientType, int Degree>
  Polynomial<CoefficientType, Degree>::Polynomial()
      : m_Coefficients{new CoefficientType[Degree + 1]}
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
  Polynomial<CoefficientType, Degree>::Polynomial(Polynomial<CoefficientType, Degree> const &_copy)
      : m_Coefficients{new CoefficientType[Degree + 1]}
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
  Polynomial<CoefficientType, Degree> &Polynomial<CoefficientType, Degree>::operator=(Polynomial<CoefficientType, Degree> const &_copy)
  {
    for (int i = 0; i <= Degree; i++)
    {
      m_Coefficients[i] = _copy[i];
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
  Polynomial<CoefficientType, Degree2> Polynomial<CoefficientType, Degree>::operator*(Polynomial<CoefficientType, Degree2> const &_rhs) const
  {
    Polynomial<CoefficientType, Degree> result;
    for (int i = 0; i <= Degree; i++)
    {
      for (int j = 0; j <= Degree; j++)
      {
        result[i + j] += m_Coefficients[i] * _rhs[j];
      }
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
