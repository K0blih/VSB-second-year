describe('Currency converter', () => {
  it('shows the converter shell', () => {
    cy.visit('/');
    cy.contains('CNB Currency Converter');
    cy.contains('Convert CZK on the fly');
    cy.contains('Choose Currency');
  });
});
