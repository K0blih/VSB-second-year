describe('My First Test', () => {
  it('Visits the app root url', () => {
    cy.visit('/')
    cy.contains('Dashboard polohy a navigace')
  })
})
